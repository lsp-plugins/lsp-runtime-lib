/*
 * ACMStream.cpp
 *
 *  Created on: 24 апр. 2020 г.
 *      Author: sadko
 */

#include <lsp-plug.in/common/alloc.h>
#include <lsp-plug.in/common/debug.h>
#include <lsp-plug.in/common/endian.h>
#include <lsp-plug.in/runtime/LSPString.h>
#include <lsp-plug.in/mm/sample.h>
#include <private/mm/ACMStream.h>
#include <stdlib.h>

#ifndef USE_LIBSNDFILE

#include <mmreg.h>
#include <msacm.h>

namespace lsp
{
    namespace mm
    {
        
        ACMStream::ACMStream()
        {
            pFmtIn      = NULL;
            pFmtOut     = NULL;
            hDriver     = NULL;
            hStream     = NULL;
            pHeader     = NULL;
        }
        
        ACMStream::~ACMStream()
        {
            close();
        }

        void ACMStream::acm_query_formats(drv_lookup_t *s)
        {
            ACMFORMATTAGDETAILSW *ftd = &s->ftd;
            ACMFORMATDETAILSW *fdw = &s->fd;

            // Iterate all formats
            for (size_t i=0, n=ftd->cStandardFormats; i < n; ++i)
            {
                ::ZeroMemory(fdw, sizeof(ACMFORMATDETAILSW));

                fdw->cbStruct       = sizeof(ACMFORMATDETAILSW);
                fdw->dwFormatTag    = ftd->dwFormatTag;
                fdw->dwFormatIndex  = i;
                fdw->pwfx           = s->req;
                fdw->cbwfx          = s->req_sz;

                if (::acmFormatDetailsW(s->hd, fdw, ACM_FORMATDETAILSF_INDEX) != 0)
                    continue;

                // Create copy and add to driver list
                if ((s->pfmt = s->ptag->vfmt.add()) == NULL)
                    continue;

                s->pfmt->id         = fdw->dwFormatIndex;
                s->pfmt->fdw        = fdw->fdwSupport;
                s->pfmt->wfex       = copy_fmt(fdw->pwfx);
            }
        }

        void ACMStream::acm_query_format_tags(drv_lookup_t *s)
        {
            ACMFORMATTAGDETAILSW *ftd = &s->ftd;

            for (size_t i=0, n=s->dd.cFormatTags; i < n; ++i)
            {
                // Prepare the structure
                ::ZeroMemory(ftd, sizeof(ACMFORMATTAGDETAILSW));
                ftd->cbStruct = sizeof(ACMFORMATTAGDETAILSW);
                ftd->dwFormatTagIndex = i;

                // Get information
                if (::acmFormatTagDetailsW(s->hd, ftd, ACM_FORMATTAGDETAILSF_INDEX) != 0)
                    continue;

                // Create tag descriptor
                if ((s->ptag = new fmt_tag_t()) == NULL)
                    continue;
                if (!s->pdrv->vtag.add(s->ptag))
                {
                    delete s->ptag;
                    continue;
                }
                s->ptag->id     = ftd->dwFormatTag;
                s->ptag->fdw    = ftd->fdwSupport;
                s->ptag->name.set_utf16(ftd->szFormatTag);

                // Query formats
                acm_query_formats(s);
            }
        }

        WINBOOL CALLBACK ACMStream::acm_driver_enum_cb(HACMDRIVERID hadid, DWORD_PTR dwInstance, DWORD fdwSupport)
        {
            drv_lookup_t *s = reinterpret_cast<drv_lookup_t *>(dwInstance);

            // Get driver information
            ACMDRIVERDETAILSW *dd = &s->dd;
            ::ZeroMemory(dd, sizeof(ACMDRIVERDETAILSW));
            dd->cbStruct    = sizeof(ACMDRIVERDETAILSW);
            if (::acmDriverDetailsW(hadid, dd, 0) != 0)
                return TRUE;

            // Open driver
            if (::acmDriverOpen(&s->hd, hadid, 0) != 0)
                return TRUE;

            // Allocate driver object
            if ((s->pdrv = new drv_t()) == NULL)
            {
                ::acmDriverClose(s->hd, 0);
                return TRUE;
            }
            if (!s->vdrv.add(s->pdrv))
            {
                delete s->pdrv;
                ::acmDriverClose(s->hd, 0);
                return TRUE;
            }
            s->pdrv->drv_id = hadid;
            s->pdrv->short_name.set_utf16(dd->szShortName);
            s->pdrv->full_name.set_utf16(dd->szLongName);
            s->pdrv->copyright.set_utf16(dd->szCopyright);
            s->pdrv->license.set_utf16(dd->szLicensing);
            s->pdrv->features.set_utf16(dd->szFeatures);

            // Query format metrics
            s->req_sz       = 0;
            ::acmMetrics((HACMOBJ)s->hd, ACM_METRIC_MAX_SIZE_FORMAT, &s->req_sz);
            s->req_sz       = align_size(s->req_sz, DEFAULT_ALIGN);
            s->req          = reinterpret_cast<WAVEFORMATEX *>(::malloc(s->req_sz));
            if (s->req != NULL)
            {
                // Query format tags
                acm_query_format_tags(s);

                ::free(s->req);
                s->req      = NULL;
            }

            // Close driver
            ::acmDriverClose(s->hd, 0);
            s->hd = NULL;

            return TRUE;
        }

        status_t ACMStream::acm_enum_drivers(lltl::parray<drv_t> *res)
        {
            drv_lookup_t lk;
            MMRESULT result = ::acmDriverEnum(acm_driver_enum_cb, (DWORD_PTR)&lk, ACM_DRIVERENUMF_DISABLED);
            if (result == 0)
            {
                res->swap(&lk.vdrv);
                acm_destroy_drivers(&lk.vdrv);
            }

            for (size_t i=0, n=res->size(); i<n; ++i)
            {
            #ifdef LSP_TRACE
                LSPString info, idx;
                drv_t *drv = res->uget(i);
                info.fmt_utf8(
                    "DRV #%-4d : %p\n"
                    "Short Name: %s\n"
                    "Long Name : %s\n"
                    "Copyright : %s\n"
                    "License   : %s\n"
                    "Features  : %s\n"
                    "Formats   :\n",
                    int(i),
                    drv->drv_id,
                    drv->short_name.get_utf8(),
                    drv->full_name.get_utf8(),
                    drv->copyright.get_utf8(),
                    drv->license.get_utf8(),
                    drv->features.get_utf8()
                );
                for (size_t j=0, m=drv->vtag.size(); j<m; ++j)
                {
                    fmt_tag_t *tag = drv->vtag.uget(j);
                    info.fmt_append_utf8("  %s [0x%x]: %d items\n",
                            tag->name.get_utf8(),
                            int(tag->id),
                            int(tag->vfmt.size())
                        );

                    for (size_t k=0, l=tag->vfmt.size(); k<l; ++k)
                    {
                        fmt_t *fmt = tag->vfmt.uget(k);
                        idx.fmt_ascii("%d/%d", int(k), int(j));
                        info.fmt_append_utf8(
                            "%10s: %s %d Hz/%d ch @%d bit [0x%x:%d]\n",
                            idx.get_utf8(),
                            tag->name.get_utf8(),
                            int(fmt->wfex->nSamplesPerSec),
                            int(fmt->wfex->nChannels),
                            int(fmt->wfex->wBitsPerSample),
                            int(fmt->wfex->wFormatTag),
                            int(fmt->wfex->cbSize)
                        );
                    }
                }
                lsp_trace("\n%s\n", info.get_native());
            #endif
            }

            return (result == 0) ? STATUS_OK : STATUS_UNKNOWN_ERR;
        }

        ACMSTREAMHEADER *ACMStream::acm_create_header(size_t in, size_t out)
        {
            size_t hdr_alloc            = align_size(sizeof(ACMSTREAMHEADER), DEFAULT_ALIGN);
            size_t src_alloc            = align_size(in, DEFAULT_ALIGN);
            size_t dst_alloc            = align_size(out, DEFAULT_ALIGN);
            BYTE *buf                   = static_cast<BYTE *>(::malloc(hdr_alloc + src_alloc + dst_alloc));

            ACMSTREAMHEADER *sh         = reinterpret_cast<ACMSTREAMHEADER *>(buf);
            if (sh != NULL)
            {
                ::ZeroMemory(sh, hdr_alloc + src_alloc + dst_alloc);

                sh->cbStruct        = sizeof(ACMSTREAMHEADER);
                sh->pbSrc                   = &buf[hdr_alloc];
                sh->cbSrcLength             = in;
                sh->cbSrcLengthUsed         = 0;
                sh->pbDst                   = &buf[hdr_alloc + src_alloc];
                sh->cbDstLength             = out;
                sh->cbDstLengthUsed         = 0;
            }

            return sh;
        }

        void ACMStream::acm_destroy_drivers(lltl::parray<drv_t> *res)
        {
            // Drop drivers
            for (size_t i=0, n=res->size(); i<n; ++i)
            {
                drv_t *pdrv = res->uget(i);

                // Drop tags
                for (size_t j=0, m=pdrv->vtag.size(); j<m; ++j)
                {
                    fmt_tag_t *ptag = pdrv->vtag.uget(j);
                    // Drop formats
                    for (size_t k=0, l=ptag->vfmt.size(); k<l; ++k)
                    {
                        fmt_t *pfmt = ptag->vfmt.uget(k);
                        if (pfmt->wfex != NULL)
                        {
                            ::free(pfmt->wfex);
                            pfmt->wfex = NULL;
                        }
                    }

                    delete ptag;
                }

                delete pdrv;
            }

            res->flush();
        }

        WAVEFORMATEX *ACMStream::copy_fmt(const WAVEFORMATEX *src)
        {
            /*
             * cbSize
             *   Size, in bytes, of extra format information appended to the end
             *   of the WAVEFORMATEX structure. This information can be used by
             *   non-PCM formats to store extra attributes for the wFormatTag. If
             *   no extra information is required by the wFormatTag, this member must
             *   be set to 0. For WAVE_FORMAT_PCM formats (and only WAVE_FORMAT_PCM formats),
             *   this member is ignored. When this structure is included in a WAVEFORMATEXTENSIBLE
             *   structure, this value must be at least 22.
             */
            size_t cbsize   = (src->wFormatTag == WAVE_FORMAT_PCM) ? 0 : src->cbSize;
            size_t bytes    = sizeof(WAVEFORMATEX) + cbsize;
            WAVEFORMATEX *res = static_cast<WAVEFORMATEX *>(::malloc(align_size(bytes, DEFAULT_ALIGN)));
            if (res != NULL)
            {
                ::memcpy(res, src, bytes);
                res->cbSize     = cbsize;   // Override original cbSize for PCM
            }
            return res;
        }

        ACMStream::fmt_tag_t *ACMStream::acm_find_tag(drv_t *drv, size_t fmt_tag)
        {
            for (size_t i=0, n=drv->vtag.size(); i<n; ++i)
            {
                fmt_tag_t *tag = drv->vtag.uget(i);
                if (tag == NULL)
                    continue;
                if ((tag->id == fmt_tag) && (tag->fdw & ACMDRIVERDETAILS_SUPPORTF_CODEC))
                    return tag;
            }
            return NULL;
        }

        status_t ACMStream::acm_configure_stream(WAVEFORMATEX *dst, WAVEFORMATEX *src)
        {
            MMRESULT res;

            // Succeeded, initialize stream
            // Estimate size of conversion buffers
            DWORD src_length = IO_BUF_SIZE, dst_length = IO_BUF_SIZE;
            if ((dst->wFormatTag == WAVE_FORMAT_PCM) || (dst->wFormatTag == WAVE_FORMAT_IEEE_FLOAT))
            {
                res = ::acmStreamSize(hStream, IO_BUF_SIZE, &dst_length, ACM_STREAMSIZEF_SOURCE);
                if ((res != 0) || (dst_length <= 0))
                    return STATUS_SKIP;

                res = ::acmStreamSize(hStream, dst_length, &src_length, ACM_STREAMSIZEF_DESTINATION);
                if ((res != 0) || (src_length <= 0))
                    return STATUS_SKIP;
            }
            else
            {
                res = ::acmStreamSize(hStream, IO_BUF_SIZE, &src_length, ACM_STREAMSIZEF_SOURCE);
                if ((res != 0) || (src_length <= 0))
                    return STATUS_SKIP;

                res = ::acmStreamSize(hStream, src_length, &dst_length, ACM_STREAMSIZEF_DESTINATION);
                if ((res != 0) || (src_length <= 0))
                    return STATUS_SKIP;
            }

            // Allocate stream header
            size_t hdr_alloc            = align_size(sizeof(ACMSTREAMHEADER), DEFAULT_ALIGN);
            size_t src_alloc            = align_size(src_length, DEFAULT_ALIGN);
            size_t dst_alloc            = align_size(dst_length, DEFAULT_ALIGN);
            BYTE *buf                   = static_cast<BYTE *>(::malloc(hdr_alloc + src_alloc + dst_alloc));
            if (buf == NULL)
                return STATUS_NO_MEM;

            pHeader                     = reinterpret_cast<ACMSTREAMHEADER *>(buf);
            ::ZeroMemory(pHeader, sizeof(ACMSTREAMHEADER));
            pHeader->cbStruct           = sizeof(ACMSTREAMHEADER);
            pHeader->pbSrc              = &buf[hdr_alloc];
            pHeader->cbSrcLength        = src_length;
            pHeader->pbDst              = &pHeader->pbSrc[src_alloc];
            pHeader->cbDstLength        = dst_length;

            // Prepare stream header
            if ((res = ::acmStreamPrepareHeader(hStream, pHeader, 0)) != 0)
                return STATUS_UNKNOWN_ERR;

            return STATUS_OK;
        }

        status_t ACMStream::acm_try_format(WAVEFORMATEX *test, WAVEFORMATEX *fmt)
        {
            status_t res;

            for (size_t i=0, n=vDrv.size(); i<n; ++i)
            {
                // Work with drivers that ONLY support the requested tag
                drv_t *drv = vDrv.uget(i);
                if (drv == NULL)
                    continue;
                fmt_tag_t *tag = acm_find_tag(drv, fmt->wFormatTag);
                if (tag == NULL)
                    continue;

                // Open the specific ACM driver
                if (::acmDriverOpen(&hDriver, drv->drv_id, 0) != 0)
                    continue;

                // Try to open stream
                if (::acmStreamOpen(&hStream, hDriver, fmt, test, NULL, 0, 0, ACM_STREAMOPENF_NONREALTIME) == 0)
                {
                    res = acm_configure_stream(test, fmt);
                    if (res == STATUS_OK)
                    {
                        // Copy format descriptors
                        if ((pFmtIn = copy_fmt(fmt)) == NULL)
                            return STATUS_NO_MEM;
                        if ((pFmtOut = copy_fmt(test)) == NULL)
                            return STATUS_NO_MEM;

                        return STATUS_OK;
                    }
                    else if (res != STATUS_SKIP)
                        return res;
                }

                // Close ACM driver
                ::acmDriverClose(hDriver, 0);
                hDriver     = NULL;
            }

            return STATUS_OK;
        }

        status_t ACMStream::acm_find_standard_dec(WAVEFORMATEX *fmt)
        {
            // TODO
            return STATUS_OK;
        }

        status_t ACMStream::acm_find_nonstandard_dec(WAVEFORMATEX *fmt)
        {
            status_t res;
            WAVEFORMATEX pcm;

            pcm.wFormatTag      = WAVE_FORMAT_IEEE_FLOAT;
            pcm.nChannels       = fmt->nChannels;
            pcm.nSamplesPerSec  = fmt->nSamplesPerSec;
            pcm.nAvgBytesPerSec = sizeof(f32_t) * fmt->nChannels * fmt->nSamplesPerSec;
            pcm.nBlockAlign     = sizeof(f32_t) * fmt->nChannels;
            pcm.wBitsPerSample  = sizeof(f32_t) * 8;
            pcm.cbSize          = 0;

            // Try IEEE Float first as the best output format
            if ((res = acm_try_format(&pcm, fmt)) != STATUS_OK)
                return res;
            else if (hStream != NULL)
                return STATUS_OK;

            // No success, try 32, 24, 16, 8 bits per sample
            for (size_t bps=4; bps > 0; --bps)
            {
                pcm.wFormatTag      = WAVE_FORMAT_PCM;
                pcm.nChannels       = fmt->nChannels;
                pcm.nSamplesPerSec  = fmt->nSamplesPerSec;
                pcm.nAvgBytesPerSec = bps * fmt->nChannels * fmt->nSamplesPerSec;
                pcm.nBlockAlign     = bps * fmt->nChannels;
                pcm.wBitsPerSample  = bps * 8;
                pcm.cbSize          = 0;

                // Try non-standard modes
                if ((res = acm_try_format(&pcm, fmt)) != STATUS_OK)
                    return res;
                if (hStream != NULL)
                    return STATUS_OK;
            }

            return STATUS_OK;
        }

        status_t ACMStream::read_pcm(WAVEFORMATEX *in)
        {
            status_t res = close();
            if (res != STATUS_OK)
                return res;

            // We do not support PCM and IEEE FLOAT format since it doesn't require decoding/encoding
            if ((in->wFormatTag == WAVE_FORMAT_PCM) || (in->wFormatTag == WAVE_FORMAT_IEEE_FLOAT))
                return STATUS_UNSUPPORTED_FORMAT;

            // Enumerate all drivers available in the system
            acm_enum_drivers(&vDrv);

            if ((res = acm_find_nonstandard_dec(in)) != STATUS_OK)
                return close(res);
            if (hStream == NULL)
            {
                if ((res = acm_find_standard_dec(in)) != STATUS_OK)
                    return close(res);
            }

            acm_destroy_drivers(&vDrv);

            return (hStream != NULL) ? STATUS_OK : close(STATUS_UNSUPPORTED_FORMAT);
        }

        status_t ACMStream::write_pcm(WAVEFORMATEX *out)
        {
            status_t res = close();
            if (res != STATUS_OK)
                return res;

            // Enumerate all drivers available in the system
            lltl::parray<drv_t> vdrv;
            acm_enum_drivers(&vdrv);

            // TODO: Choose the corresponding driver
            acm_destroy_drivers(&vdrv);
            return STATUS_OK;
        }

        status_t ACMStream::close(status_t res)
        {
            // Destroy information about all drivers
            acm_destroy_drivers(&vDrv);

            // Close stream
            if (hStream != NULL)
            {
                // Unprepare ACM header first
                if ((pHeader != NULL) && (pHeader->fdwStatus != 0))
                    ::acmStreamUnprepareHeader(hStream, pHeader, 0);

                ::acmStreamClose(hStream, 0);
                hStream     = NULL;
            }

            // Close driver
            if (hDriver != NULL)
            {
                ::acmDriverClose(hDriver, 0);
                hDriver     = NULL;
            }

            // Free allocated memory
            if (pFmtIn != NULL)
            {
                ::free(pFmtIn);
                pFmtIn = NULL;
            }

            if (pFmtOut != NULL)
            {
                ::free(pFmtOut);
                pFmtOut = NULL;
            }

            if (pHeader != NULL)
            {
                ::free(pHeader);
                pHeader = NULL;
            }

            return res;
        }

        ssize_t ACMStream::push(void **buf)
        {
            if (pHeader == NULL)
                return -STATUS_CLOSED;
            size_t avail = pHeader->cbSrcLength - pHeader->cbSrcLengthUsed;
            if (buf != NULL)
                *buf = &pHeader->pbSrc[pHeader->cbSrcLengthUsed];
            return avail;
        }

        ssize_t ACMStream::pull(void *buf, size_t size, bool force)
        {
            if (hStream == NULL)
                return -STATUS_CLOSED;

            size_t avail = (pHeader->cbDstLength - pHeader->cbDstLengthUsed);

            // There should be at least 3/4 free in the buffer to call acmStreamConvert
            if (avail < ((pHeader->cbDstLength * 3) >> 2))
            {
                size_t flags = (force) ? ACM_STREAMCONVERTF_BLOCKALIGN : 0;
                if (::acmStreamConvert(hStream, pHeader, flags) != 0)
                    return -STATUS_IO_ERROR;
            }

            // Just pull the data to buffer
            if (size > avail)
                size    = avail;
            avail -= size;
            ::memcpy(buf, pHeader->pbDst, size);
            if (avail > 0)
                ::memmove(pHeader->pbDst, &pHeader->pbDst[size], avail);
            pHeader->cbDstLengthUsed = avail;

            return size;
        }

        size_t ACMStream::avail()
        {
            if (pHeader == NULL)
                return 0;
            return pHeader->cbDstLengthUsed;
        }

        /*
            LONG            error;
            WAVEFORMATEX    dstInfo;

            lltl::parray<acm_driver_t> acmDrivers;
            ::acmDriverEnum(acm_driver_enum_callback, (DWORD_PTR)&acmDrivers, ACM_DRIVERENUMF_DISABLED);
            for (size_t i=0, n=acmDrivers.size(); i<n; ++i)
            {
                acm_driver_t *drv = acmDrivers.uget(i);
                printf("ACM Driver=%p, support=0x%lx\n", drv->hDrvId, long(drv->fdwSupport));
            }

            // Update sample format
            sFormat.srate           = LE_TO_CPU(pWfexInfo->nSamplesPerSec);
            sFormat.channels        = LE_TO_CPU(pWfexInfo->nChannels);
            sFormat.frames          = -1;
            sFormat.format          = mm::SFMT_F32_CPU;

            // We are ready to read but first initialize ACM stream
            dstInfo.wFormatTag      = WAVE_FORMAT_IEEE_FLOAT;
            dstInfo.nChannels       = sFormat.channels;
            dstInfo.nSamplesPerSec  = sFormat.srate;
            dstInfo.nAvgBytesPerSec = sFormat.srate * sFormat.channels * sizeof(float);
            dstInfo.nBlockAlign     = sFormat.channels * sizeof(float);
            dstInfo.wBitsPerSample  = sizeof(float) * 8;
            dstInfo.cbSize          = 0;

            dstInfo.wFormatTag      = CPU_TO_LE(dstInfo.wFormatTag);
            dstInfo.nChannels       = CPU_TO_LE(dstInfo.nChannels);
            dstInfo.nSamplesPerSec  = CPU_TO_LE(dstInfo.nSamplesPerSec);
            dstInfo.nAvgBytesPerSec = CPU_TO_LE(dstInfo.nAvgBytesPerSec);
            dstInfo.nBlockAlign     = CPU_TO_LE(dstInfo.nBlockAlign);
            dstInfo.wBitsPerSample  = CPU_TO_LE(dstInfo.wBitsPerSample);
            dstInfo.cbSize          = CPU_TO_LE(dstInfo.cbSize);

            // Open and configure ACM stream
            HACMSTREAM acmStream;
            if ((error = ::acmStreamOpen(&acmStream, NULL, pWfexInfo, &dstInfo, NULL, 0, 0, ACM_STREAMOPENF_NONREALTIME)) != 0)
            {
                switch (error)
                {
                    case ACMERR_NOTPOSSIBLE: return STATUS_BAD_FORMAT;
                    case STATUS_NO_MEM: return STATUS_NO_MEM;
                    default: break;
                }
                return STATUS_UNKNOWN_ERR;
            }

            // Estimate size of conversion buffers
            DWORD src_length = ACM_INPUT_BUFSIZE, dst_length = ACM_INPUT_BUFSIZE;

            error = ::acmStreamSize(acmStream, ACM_INPUT_BUFSIZE, &dst_length, ACM_STREAMSIZEF_SOURCE);
            if ((error != 0) || (dst_length <= 0))
            {
                ::acmStreamClose(acmStream, 0);
                return STATUS_UNKNOWN_ERR;
            }
            error = ::acmStreamSize(acmStream, dst_length, &src_length, ACM_STREAMSIZEF_DESTINATION);
            if ((error != 0) || (src_length <= 0))
            {
                ::acmStreamClose(acmStream, 0);
                return STATUS_UNKNOWN_ERR;
            }

            // Allocate stream header
            size_t hdr_alloc            = align_size(sizeof(ACMSTREAMHEADER), DEFAULT_ALIGN);
            size_t src_alloc            = align_size(src_length, DEFAULT_ALIGN);
            size_t dst_alloc            = align_size(dst_length, DEFAULT_ALIGN);
            BYTE *buf                   = static_cast<BYTE *>(::malloc(hdr_alloc + src_alloc + dst_alloc));

            ACMSTREAMHEADER *sh         = reinterpret_cast<ACMSTREAMHEADER *>(buf);
            if (sh == NULL)
            {
                ::acmStreamClose(acmStream, 0);
                return STATUS_NO_MEM;
            }

            ::bzero(sh, sizeof(ACMSTREAMHEADER));
            sh->pbSrc                   = &buf[hdr_alloc];
            sh->cbSrcLength             = src_length;
            sh->pbDst                   = &buf[hdr_alloc + src_alloc];
            sh->cbDstLength             = dst_length;

            // Prepare stream header
            if ((error = acmStreamPrepareHeader( acmStream, sh, 0 )) != 0)
            {
                ::free(buf);
                ::acmStreamClose(acmStream, 0);
                return STATUS_BAD_FORMAT;
            }

            */
    
    } /* namespace mm */
} /* namespace lsp */

#endif /* USE_LIBSNDFILE */
