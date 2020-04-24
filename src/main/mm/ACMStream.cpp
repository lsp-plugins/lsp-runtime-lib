/*
 * ACMStream.cpp
 *
 *  Created on: 24 апр. 2020 г.
 *      Author: sadko
 */

#include <lsp-plug.in/common/alloc.h>
#include <lsp-plug.in/common/debug.h>
#include <lsp-plug.in/runtime/LSPString.h>
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
        }
        
        ACMStream::~ACMStream()
        {
            close();
        }

        static void append_fourcc(LSPString &s, FOURCC fcc)
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

                // Debug
                LSPString info;
                info.fmt_append_ascii("  fmttag idx=%d, tag=%x, size=%d, fdw=%x, nfmt=%d, name=",
                        int(ftd->dwFormatTagIndex),
                        int(ftd->dwFormatTag),
                        int(ftd->cbFormatSize),
                        int(ftd->fdwSupport),
                        int(ftd->cStandardFormats)
                    );

                info.append_utf16(ftd->szFormatTag);
                lsp_trace("%s\n", info.get_native());

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
            s->drv_id       = hadid;

            // Get driver information
            ACMDRIVERDETAILSW *dd = &s->dd;
            ::ZeroMemory(dd, sizeof(ACMDRIVERDETAILSW));
            dd->cbStruct    = sizeof(ACMDRIVERDETAILSW);
            if (::acmDriverDetailsW(hadid, dd, 0) != 0)
                return TRUE;

            LSPString info;
            info.fmt_append_ascii("ACM Driver=%p, support=0x%lx\n", hadid, long(fdwSupport));
            info.append_ascii("  fccType = "); append_fourcc(info, dd->fccType);
            info.append_ascii(", fccComp = "); append_fourcc(info, dd->fccComp);
            info.append('\n');
            info.append_ascii("  short = "); info.append_utf16(dd->szShortName); info.append('\n');
            info.append_ascii("  long  = "); info.append_utf16(dd->szLongName); info.append('\n');
            info.append_ascii("  copy  = "); info.append_utf16(dd->szCopyright); info.append('\n');
            info.append_ascii("  lic   = "); info.append_utf16(dd->szLicensing); info.append('\n');
            info.append_ascii("  feat  = "); info.append_utf16(dd->szFeatures); info.append('\n');

            lsp_trace("%s\n", info.get_native());

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

            return (result == 0) ? STATUS_OK : STATUS_UNKNOWN_ERR;
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

        status_t ACMStream::read_pcm(WAVEFORMATEX *in)
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

        status_t ACMStream::close()
        {
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

            return STATUS_OK;
        }
    
    } /* namespace mm */
} /* namespace lsp */

#endif /* USE_LIBSNDFILE */
