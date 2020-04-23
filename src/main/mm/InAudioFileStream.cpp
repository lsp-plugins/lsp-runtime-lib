/*
 * IInAudioFileStream.cpp
 *
 *  Created on: 19 апр. 2020 г.
 *      Author: sadko
 */

#include <lsp-plug.in/mm/InAudioFileStream.h>
#include <lsp-plug.in/common/endian.h>
#include <lsp-plug.in/common/alloc.h>
#include <lsp-plug.in/common/debug.h>
#include <lsp-plug.in/stdlib/stdio.h>
#include <lsp-plug.in/lltl/parray.h>

#ifdef USE_LIBSNDFILE
    #if (__SIZEOF_INT__ == 4)
        #define AFS_S32_CPU     mm::SFMT_S32_CPU
    #endif

    #if (__SIZEOF_SHORT__ == 2)
        #define AFS_S16_CPU     mm::SFMT_S16_CPU
    #elif ((__SIZEOF_SHORT__ == 4) && (!defined(AFS_S32_CPU)))
        #define AFS_S32_CPU     mm::SFMT_S32_CPU
    #endif
#else
    #define ACM_INPUT_BUFSIZE       0x1000
#endif /* USE_LIBSNDFILE */

namespace lsp
{
    namespace mm
    {
        
        InAudioFileStream::InAudioFileStream()
        {
        #ifdef USE_LIBSNDFILE
            hHandle     = NULL;
            bSeekable   = false;
        #else
            hMMIO       = NULL;
            hACM        = NULL;
            pMmioInfo   = NULL;
            pAHead      = NULL;
            pCkInRiff   = NULL;
            pWfexInfo   = NULL;
        #endif /* USE_LIBSNDFILE */
        }
        
        InAudioFileStream::~InAudioFileStream()
        {
            IInAudioStream::close();
            close_handle();
        }

        status_t InAudioFileStream::close_handle()
        {
        #ifdef USE_LIBSNDFILE
            if (hHandle == NULL)
                return STATUS_OK;

            int res     = sf_close(hHandle);

            hHandle     = NULL;
            bSeekable   = false;
            nOffset     = -1;       // Mark as closed

            return set_error((res == 0) ? STATUS_OK : STATUS_IO_ERROR);
        #else
            if (hACM != NULL)
            {
                ::acmStreamClose(hACM, 0);
                hACM        = NULL;
            }

            if (hMMIO != NULL)
            {
                ::mmioClose(hMMIO, 0);
                hMMIO       = NULL;
            }

            if (pMmioInfo != NULL)
            {
                ::free(pMmioInfo);
                pMmioInfo   = NULL;
            }
            if (pAHead != NULL)
            {
                ::free(pAHead);
                pAHead      = NULL;
            }
            if (pCkInRiff != NULL)
            {
                ::free(pCkInRiff);
                pCkInRiff   = NULL;
            }
            if (pWfexInfo != NULL)
            {
                ::free(pWfexInfo);
                pWfexInfo   = NULL;
            }

            nOffset     = -1;       // Mark as closed

            return set_error(STATUS_OK);
        #endif /* USE_LIBSNDFILE */
        }

    #ifdef USE_LIBSNDFILE
        status_t InAudioFileStream::decode_sf_error(SNDFILE *fd)
        {
            switch (sf_error(fd))
            {
                case SF_ERR_NO_ERROR:
                    return STATUS_OK;
                case SF_ERR_UNRECOGNISED_FORMAT:
                    return STATUS_BAD_FORMAT;
                case SF_ERR_MALFORMED_FILE:
                    return STATUS_CORRUPTED_FILE;
                case SF_ERR_UNSUPPORTED_ENCODING:
                    return STATUS_BAD_FORMAT;
                default:
                    return STATUS_UNKNOWN_ERR;
            }
        }
    #endif /* USE_LIBSNDFILE */

        status_t InAudioFileStream::open(const char *path)
        {
            if (path == NULL)
                return set_error(STATUS_BAD_ARGUMENTS);
            LSPString xpath;
            if (!xpath.set_utf8(path))
                return set_error(STATUS_NO_MEM);
            return open(&xpath);
        }

        status_t InAudioFileStream::open(const io::Path *path)
        {
            if (path == NULL)
                return set_error(STATUS_BAD_ARGUMENTS);
            return open(path->as_string());
        }

    #ifndef USE_LIBSNDFILE
        status_t InAudioFileStream::open_riff_file(const LSPString *path)
        {
            HMMIO           hmmioIn;
            MMCKINFO        ckIn;
            MMCKINFO        ckInRiff;
            MMIOINFO        mmioInfo;
            PCMWAVEFORMAT   pcmWaveFormat;
            int             error;

            if ((hmmioIn = ::mmioOpenW(const_cast<WCHAR *>(path->get_utf16()), NULL, MMIO_ALLOCBUF | MMIO_READ)) == NULL)
                return STATUS_PERMISSION_DENIED;

            if ((error = int(::mmioDescend(hmmioIn, &ckInRiff, NULL, 0))) != 0)
            {
                ::mmioClose(hmmioIn, 0);
                return STATUS_BAD_FORMAT;
            }

            FOURCC ckid         = LE_TO_CPU(ckInRiff.ckid);
            size_t fccType      = LE_TO_CPU(ckInRiff.fccType);
            if ((ckid != FOURCC_RIFF) || (fccType != mmioFOURCC('W', 'A', 'V', 'E')))
            {
                ::mmioClose(hmmioIn, 0);
                return STATUS_BAD_FORMAT;
            }

            ckid                = mmioFOURCC('f', 'm', 't', ' ');
            ckIn.ckid           = CPU_TO_LE(ckid);
            if ((error = int(::mmioDescend(hmmioIn, &ckIn, &ckInRiff, MMIO_FINDCHUNK))) != 0)
            {
                ::mmioClose(hmmioIn, 0);
                return STATUS_BAD_FORMAT;
            }

            // Expect the 'fmt' chunk to be at least as large as sizeof(PCMWAVEFORMAT)
            size_t cksize   = LE_TO_CPU(ckIn.cksize);
            if (cksize < sizeof(PCMWAVEFORMAT))
            {
                ::mmioClose(hmmioIn, 0);
                return STATUS_BAD_FORMAT;
            }

            // Read the 'fmt ' chunk
            size_t bytes = ::mmioRead(hmmioIn, (HPSTR) &pcmWaveFormat, sizeof(PCMWAVEFORMAT));
            if (bytes != sizeof(PCMWAVEFORMAT))
            {
                ::mmioClose(hmmioIn, 0);
                return STATUS_BAD_FORMAT;
            }

            // Estimate number of bytes to allocate for the format descriptor
            WAVEFORMATEX *wfex  = NULL;
            size_t fmtTag       = LE_TO_CPU(pcmWaveFormat.wf.wFormatTag);

            if (fmtTag != WAVE_FORMAT_PCM)
            {
                WORD cbSize;
                size_t alloc        = sizeof(WAVEFORMATEX);

                // Read in length of extra bytes.
                bytes = ::mmioRead(hmmioIn, reinterpret_cast<HPSTR>(&cbSize), sizeof(WORD));
                if (bytes != sizeof(WORD))
                {
                    ::mmioClose(hmmioIn, 0);
                    return STATUS_BAD_FORMAT;
                }
                alloc              += LE_TO_CPU(cbSize);

                // Allocate memory
                wfex = static_cast<WAVEFORMATEX *>(::malloc(alloc));
                if (wfex == NULL)
                {
                    ::mmioClose(hmmioIn, 0);
                    return STATUS_NO_MEM;
                }

                // Return back to 'fmt ' chunk
                if ((error = int(::mmioDescend(hmmioIn, &ckIn, &ckInRiff, MMIO_FINDCHUNK))) != 0)
                {
                    ::mmioClose(hmmioIn, 0);
                    return STATUS_BAD_FORMAT;
                }

                // Read the whole structure again
                bytes = ::mmioRead(hmmioIn, reinterpret_cast<HPSTR>(wfex), alloc);
                if (bytes != alloc)
                {
                    ::mmioClose(hmmioIn, 0);
                    return STATUS_BAD_FORMAT;
                }
            }
            else
            {
                // Allocate memory
                wfex = static_cast<WAVEFORMATEX *>(::malloc(sizeof(WAVEFORMATEX)));
                if (wfex == NULL)
                {
                    ::mmioClose(hmmioIn, 0);
                    return STATUS_NO_MEM;
                }
                ::memcpy(wfex, &pcmWaveFormat, sizeof(PCMWAVEFORMAT));
                wfex->cbSize = 0;
            }

            /* Ascend the input file out of the 'fmt ' chunk. */
//            if ((error = ::mmioAscend(hmmioIn, &ckIn, 0)) != 0)
//            {
//                ::mmioClose(hmmioIn, 0);
//                return STATUS_BAD_FORMAT;
//            }

            // Perform a seek to data
            if ((error = ::mmioSeek(hmmioIn, LE_TO_CPU(ckInRiff.dwDataOffset) + sizeof(FOURCC), SEEK_SET)) == -1)
                return STATUS_BAD_FORMAT;

            ckid            = mmioFOURCC('d', 'a', 't', 'a');
            ckIn.ckid       = CPU_TO_LE(ckid);
            if ((error = ::mmioDescend(hmmioIn, &ckIn, &ckInRiff, MMIO_FINDCHUNK)) != 0)
                return STATUS_BAD_FORMAT;

            if ((error = ::mmioGetInfo(hmmioIn, &mmioInfo, 0)) != 0)
                return STATUS_IO_ERROR;

            // Deploy state
            pCkInRiff   = static_cast<MMCKINFO *>(lsp::memdup(&ckInRiff, sizeof(MMCKINFO)));
            if (pCkInRiff == NULL)
            {
                ::mmioClose(hmmioIn, 0);
                return STATUS_NO_MEM;
            }

            pMmioInfo   = static_cast<MMIOINFO *>(lsp::memdup(&mmioInfo, sizeof(MMIOINFO)));
            if (pMmioInfo == NULL)
            {
                ::free(pCkInRiff);
                pCkInRiff       = NULL;
                ::mmioClose(hmmioIn, 0);
                return STATUS_NO_MEM;
            }

            hMMIO       = hmmioIn;
            pWfexInfo   = wfex;

            return STATUS_OK;
        }

        typedef struct acm_fmt_tag_t
        {
            LPACMFORMATTAGDETAILSW      sDetails;
        } acm_fmt_tag_t;

        typedef struct acm_driver_t
        {
            HACMDRIVERID                        hDrvId;
            DWORD                               fdwSupport;
            ACMDRIVERDETAILSW                   sDetails;
            lltl::parray<ACMFORMATDETAILSW>     sFormats;
            lltl::parray<ACMFORMATTAGDETAILSW>  sTagFormats;
        } acm_driver_t;

        void append_fourcc(LSPString &s, FOURCC fcc)
        {
            FOURCC v = fcc;
            for (size_t i=0; i<4; ++i, v >>= 8)
            {
                char c = v & 0xff;
                if (c == '\0')
                    c = '?';
                s.append(c);
            }
            s.fmt_append_ascii(" (0x%lx)", long(fcc));
        }

//        WINBOOL CALLBACK acm_format_tag_enum_callback(HACMDRIVERID hadid, LPACMFORMATTAGDETAILSW paftd, DWORD_PTR dwInstance, DWORD fdwSupport)
//        {
//            acm_fmt_tag_t *fmt = new acm_fmt_tag_t();
//            fmt->sDetails   = paftd;
//
//            ACMFORMATTAGDETAILSW *d = paftd;
//            LSPString info;
//            info.fmt_append_ascii("  fmt: %d/%d/%d/%x/%d - ",
//                    int(d->dwFormatTagIndex),
//                    int(d->dwFormatTag),
//                    int(d->cbFormatSize),
//                    int(d->fdwSupport),
//                    int(d->cStandardFormats)
//                );
//            info.append_utf16(d->szFormatTag);
//            lsp_trace("%s\n", info.get_native());
//
//            return TRUE;
//        }

        WINBOOL CALLBACK acm_format_enum_callback(HACMDRIVERID hadid, LPACMFORMATDETAILSW pafd, DWORD_PTR dwInstance, DWORD fdwSupport)
        {
            size_t hdr          = align_size(sizeof(ACMFORMATDETAILSW), DEFAULT_ALIGN);
            size_t cbwfx        = pafd->cbwfx;

            // Make a copy of format
            BYTE *buf           = static_cast<BYTE *>(::malloc(hdr + cbwfx));
            if (buf == NULL)
                return TRUE;
            ACMFORMATDETAILSW *cfd = reinterpret_cast<ACMFORMATDETAILSW *>(buf);
            *cfd            = *pafd;
            cfd->pwfx       = reinterpret_cast<WAVEFORMATEX *>(&buf[hdr]);
            cfd->cbwfx      = cbwfx;
            ::memcpy(cfd->pwfx, pafd->pwfx, sizeof(WAVEFORMATEX) + pafd->pwfx->cbSize);

            // Add copy to list
            acm_driver_t *drv = reinterpret_cast<acm_driver_t *>(dwInstance);
            if (!drv->sFormats.add(cfd))
                ::free(cfd);

            LSPString info;
            info.fmt_append_ascii("  fmt: idx=%d, tag=%d, fdw=%d, name=",
                    int(cfd->dwFormatIndex),
                    int(cfd->dwFormatTag),
                    int(cfd->fdwSupport)
                );
            info.append_utf16(cfd->szFormat);

            WAVEFORMATEX *wf = cfd->pwfx;
            info.fmt_append_ascii("\n  det: tag=%d, chan=%d, sr=%d, abps=%d, %algn=%d, bps=%d, cbsz=%d",
                    int(wf->wFormatTag),
                    int(wf->nChannels),
                    int(wf->nSamplesPerSec),
                    int(wf->nAvgBytesPerSec),
                    int(wf->nBlockAlign),
                    int(wf->wBitsPerSample),
                    int(wf->cbSize)
                );

            lsp_trace("%s\n", info.get_native());

            return true;
        }

        void acm_format_enum(HACMDRIVER had, acm_driver_t *drv)
        {
            ACMDRIVERDETAILSW *p = &drv->sDetails;

            DWORD bytes         = 0;
            size_t hdr          = align_size(sizeof(ACMFORMATDETAILSW), DEFAULT_ALIGN);
            ::acmMetrics((HACMOBJ)had, ACM_METRIC_MAX_SIZE_FORMAT, &bytes);
            bytes       = align_size(bytes, DEFAULT_ALIGN);

            BYTE *buf           = static_cast<BYTE *>(::malloc(hdr + bytes));
            ACMFORMATDETAILSW *fdw = reinterpret_cast<ACMFORMATDETAILSW *>(buf);
            fdw->cbStruct       = sizeof(ACMFORMATDETAILSW);
            fdw->pwfx           = reinterpret_cast<WAVEFORMATEX *>(&buf[hdr]);
            fdw->dwFormatIndex  = 0;
            fdw->cbwfx          = bytes;

            for (size_t i=0; i<p->cFormatTags; ++i)
            {
                fdw->dwFormatTag          = 0;
                ::acmFormatEnumW(had, fdw, acm_format_enum_callback, (DWORD_PTR)drv, 0);
            }
            ::free(buf);
        }

        void acm_query_formats(HACMDRIVER had, ACMFORMATTAGDETAILSW *tag, acm_driver_t *drv)
        {
            ACMDRIVERDETAILSW *p = &drv->sDetails;

            DWORD bytes         = 0;
            ::acmMetrics((HACMOBJ)had, ACM_METRIC_MAX_SIZE_FORMAT, &bytes);
            bytes               = align_size(bytes, DEFAULT_ALIGN);
            size_t hdr          = align_size(sizeof(ACMFORMATDETAILSW), DEFAULT_ALIGN);

            BYTE *buf           = static_cast<BYTE *>(::malloc(hdr + bytes));
            ACMFORMATDETAILSW *fdw = reinterpret_cast<ACMFORMATDETAILSW *>(buf);

            for (size_t i=0, n=tag->cStandardFormats; i < n; ++i)
            {
                ::ZeroMemory(buf, hdr + bytes);

                fdw->cbStruct       = sizeof(ACMFORMATDETAILSW);
                fdw->dwFormatTag    = tag->dwFormatTag;
                fdw->dwFormatIndex  = i;
                fdw->pwfx           = reinterpret_cast<WAVEFORMATEX *>(&buf[hdr]);
                fdw->cbwfx          = bytes;

                MMRESULT res = ::acmFormatDetailsW(had, fdw, ACM_FORMATDETAILSF_INDEX);
                if (res != 0)
                {
                    switch (res)
                    {
                        case ACMERR_NOTPOSSIBLE: lsp_trace("error = ACMERR_NOTPOSSIBLE"); break;
                        case MMSYSERR_INVALFLAG: lsp_trace("error = MMSYSERR_INVALFLAG"); break;
                        case MMSYSERR_INVALHANDLE: lsp_trace("error = MMSYSERR_INVALHANDLE"); break;
                        case MMSYSERR_INVALPARAM: lsp_trace("error = MMSYSERR_INVALPARAM"); break;
                        default: lsp_trace("error = %d", int (res)); break;
                    }
                    continue;
                }

                // Output info
                WAVEFORMATEX *wf = fdw->pwfx;

                LSPString info;
                info.fmt_append_ascii("  fmt tag=%x, ch=%d, sps=%d, abps=%d, blka=%d bps=%d cbz=%d",
                        int(wf->wFormatTag),
                        int(wf->nChannels),
                        int(wf->nSamplesPerSec),
                        int(wf->nAvgBytesPerSec),
                        int(wf->nBlockAlign),
                        int(wf->wBitsPerSample),
                        int(wf->cbSize)
                    );

                lsp_trace("%s\n", info.get_native());
            }
        }

        void acm_query_format_tags(HACMDRIVER had, acm_driver_t *drv)
        {
            ACMDRIVERDETAILSW *p = &drv->sDetails;
            ACMFORMATTAGDETAILSW ftd;

            for (size_t i=0, n=p->cFormatTags; i < n; ++i)
            {
                // Prepare the structure
                ::ZeroMemory(&ftd, sizeof(ACMFORMATTAGDETAILSW));
                ftd.cbStruct = sizeof(ACMFORMATTAGDETAILSW);
                ftd.dwFormatTagIndex = i;

                // Get information
                MMRESULT res = ::acmFormatTagDetailsW(had, &ftd, ACM_FORMATTAGDETAILSF_INDEX);
                if (res != 0)
                {
                    switch (res)
                    {
                        case MMSYSERR_INVALFLAG: lsp_trace("error = MMSYSERR_INVALFLAG"); break;
                        case MMSYSERR_INVALHANDLE: lsp_trace("error = MMSYSERR_INVALHANDLE"); break;
                        case MMSYSERR_INVALPARAM: lsp_trace("error = MMSYSERR_INVALPARAM"); break;
                        default: lsp_trace("error = %d", int (res)); break;
                    }
                    continue;
                }

                LSPString info;
                info.fmt_append_ascii("  fmttag idx=%d, tag=%x, size=%d, fdw=%x, nfmt=%d, name=",
                        int(ftd.dwFormatTagIndex),
                        int(ftd.dwFormatTag),
                        int(ftd.cbFormatSize),
                        int(ftd.fdwSupport),
                        int(ftd.cStandardFormats)
                    );

                info.append_utf16(ftd.szFormatTag);
                lsp_trace("%s\n", info.get_native());

                // Save a copy
                ACMFORMATTAGDETAILSW *copy = static_cast<ACMFORMATTAGDETAILSW *>(memdup(&ftd, sizeof(ACMFORMATTAGDETAILSW)));
                if (copy != NULL)
                {
                    if (!drv->sTagFormats.add(copy))
                        ::free(copy);
                }

                // Query formats
                acm_query_formats(had, copy, drv);
            }
        }

        WINBOOL CALLBACK acm_driver_enum_callback(HACMDRIVERID hadid, DWORD_PTR dwInstance, DWORD fdwSupport)
        {
            lltl::parray<acm_driver_t> *list = reinterpret_cast<lltl::parray<acm_driver_t> *>(dwInstance);
            acm_driver_t *drv = new acm_driver_t();
            drv->hDrvId     = hadid;
            drv->fdwSupport = fdwSupport;

            drv->sDetails.cbStruct   = sizeof(ACMDRIVERDETAILSW);
            if (::acmDriverDetailsW(hadid, &drv->sDetails, 0) != 0)
            {
                delete drv;
                return TRUE;
            }

            LSPString info;
            ACMDRIVERDETAILSW *p = &drv->sDetails;
            info.fmt_append_ascii("ACM Driver=%p, support=0x%lx\n", hadid, long(fdwSupport));
            info.append_ascii("  fccType = "); append_fourcc(info, p->fccType);
            info.append_ascii(", fccComp = "); append_fourcc(info, p->fccComp);
            info.append('\n');
            info.append_ascii("  short = "); info.append_utf16(p->szShortName); info.append('\n');
            info.append_ascii("  long  = "); info.append_utf16(p->szLongName); info.append('\n');
            info.append_ascii("  copy  = "); info.append_utf16(p->szCopyright); info.append('\n');
            info.append_ascii("  lic   = "); info.append_utf16(p->szLicensing); info.append('\n');
            info.append_ascii("  feat  = "); info.append_utf16(p->szFeatures); info.append('\n');

            lsp_trace("%s\n", info.get_native());

            HACMDRIVER              had;
            ::acmDriverOpen(&had, hadid, 0);

            acm_query_format_tags(had, drv);

//            acm_format_enum(had, drv);

//            ACMFORMATTAGDETAILSW    ftd;
//            ftd.cbStruct        = sizeof(ACMFORMATTAGDETAILSW);
//            ::acmFormatTagEnumW(had, &ftd, acm_format_tag_enum_callback, (DWORD_PTR)drv, 0);

            if (!list->add(drv))
                delete drv;

            return TRUE;
        }

        status_t InAudioFileStream::open_acm_stream_read()
        {
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
            error = ::acmStreamSize(acmStream,dst_length, &src_length, ACM_STREAMSIZEF_DESTINATION);
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

            // Update data
            hACM        = acmStream;
            pAHead      = sh;

            return STATUS_OK;
        }
    #endif

        status_t InAudioFileStream::open(const LSPString *path)
        {
            if (!is_closed())
                return -set_error(STATUS_OPENED);

        #ifdef USE_LIBSNDFILE
            SF_INFO info;
            SNDFILE *sf;

            // Open file for reading
            if ((sf = sf_open(path->get_native(), SFM_READ, &info)) == NULL)
                return set_error(decode_sf_error(sf));

            // Decode metadata
            sFormat.srate       = info.samplerate;
            sFormat.channels    = info.channels;
            sFormat.frames      = info.frames;

            // Decode sample format
            switch (info.format & SF_FORMAT_SUBMASK)
            {
                case SF_FORMAT_PCM_U8: sFormat.format = mm::SFMT_U8_CPU; break;
                case SF_FORMAT_PCM_S8: sFormat.format = mm::SFMT_S8_CPU; break;
                case SF_FORMAT_PCM_16: sFormat.format = mm::SFMT_S16_CPU; break;
                case SF_FORMAT_PCM_24: sFormat.format = mm::SFMT_S24_CPU; break;
                case SF_FORMAT_PCM_32: sFormat.format = mm::SFMT_S32_CPU; break;
                case SF_FORMAT_FLOAT:  sFormat.format = mm::SFMT_F32_CPU; break;
                case SF_FORMAT_DOUBLE: sFormat.format = mm::SFMT_F64_CPU; break;
                default:               sFormat.format = mm::SFMT_F32_CPU; break;
            }

            // Commit new state
            hHandle             = sf;
            nOffset             = 0;
            bSeekable           = info.seekable;

            return set_error(STATUS_OK);
        #else
            status_t res = open_riff_file(path);
            if (res != STATUS_OK)
                return set_error(res);
            res = open_acm_stream_read();
            if (res != STATUS_OK)
            {
                close_handle();
                return set_error(res);
            }

            return set_error(STATUS_OK);
        #endif /* USE_LIBSNDFILE */
        }

        status_t InAudioFileStream::close()
        {
            IInAudioStream::close();
            return close_handle();
        }

        size_t InAudioFileStream::select_format(size_t fmt)
        {
        #ifdef USE_LIBSNDFILE
            switch (sformat_format(fmt))
            {
            #ifdef AFS_S32_CPU
                case SFMT_S32:
                case SFMT_U32:
                case SFMT_S24:
                case SFMT_U24:
                    return SFMT_S32_CPU;
            #endif

            #ifdef AFS_S16_CPU
                case SFMT_S16:
                case SFMT_U16:
                case SFMT_S8:
                case SFMT_U8:
                    return SFMT_S16_CPU;
            #endif

                case SFMT_F32:
                    return SFMT_F32_CPU;

                case SFMT_F64:
                    return SFMT_F64_CPU;

                default:
                    break;
            }

            return SFMT_F32_CPU;
        #endif
        }

        ssize_t InAudioFileStream::direct_read(void *dst, size_t nframes, size_t fmt)
        {
        #ifdef USE_LIBSNDFILE
            sf_count_t count;
            status_t res;

            switch (sformat_format(fmt))
            {
                case SFMT_S32:
                    count   = sf_readf_int(hHandle, static_cast<int *>(dst), nframes);
                    break;

                case SFMT_S16:
                    count   = sf_readf_short(hHandle, static_cast<short *>(dst), nframes);
                    break;

                case SFMT_F32:
                    count   = sf_readf_float(hHandle, static_cast<float *>(dst), nframes);
                    break;

                case SFMT_F64:
                    count   = sf_readf_double(hHandle, static_cast<double *>(dst), nframes);
                    break;

                default: // Force SFMT_F32 sample format
                    count   = sf_readf_float(hHandle, static_cast<float *>(dst), nframes);
                    break;
            }

            // Is there data written?
            if (count > 0)
                return count;

            res = decode_sf_error(hHandle);
            return -((res == STATUS_OK) ? STATUS_EOF : res);
        #endif /* USE_LIBSNDFILE */
        }

        wssize_t InAudioFileStream::skip(wsize_t nframes)
        {
            if (is_closed())
                return -set_error(STATUS_CLOSED);

        #ifdef USE_LIBSNDFILE
            if (!bSeekable)
                return IInAudioStream::skip(nframes);

            sf_count_t res = sf_seek(hHandle, nframes, SF_SEEK_CUR);
            if (res >= 0)
            {
                nOffset    += nframes;
                set_error(STATUS_OK);
                return nframes;
            }

            return -set_error(decode_sf_error(hHandle));
        #endif /* USE_LIBSNDFILE */
        }

        wssize_t InAudioFileStream::seek(wsize_t nframes)
        {
            if (is_closed())
                return -set_error(STATUS_CLOSED);

        #ifdef USE_LIBSNDFILE
            if (!bSeekable)
                return IInAudioStream::seek(nframes);

            sf_count_t res = sf_seek(hHandle, nframes, SF_SEEK_SET);
            if (res >= 0)
            {
                nOffset     = nframes;
                set_error(STATUS_OK);
                return nframes;
            }

            return -set_error(decode_sf_error(hHandle));
        #endif /* USE_LIBSNDFILE */
        }
    
    } /* namespace mm */
} /* namespace lsp */
