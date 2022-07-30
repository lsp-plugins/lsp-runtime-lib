/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-runtime-lib
 * Created on: 19 апр. 2020 г.
 *
 * lsp-runtime-lib is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * lsp-runtime-lib is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with lsp-runtime-lib. If not, see <https://www.gnu.org/licenses/>.
 */

#include <lsp-plug.in/mm/InAudioFileStream.h>
#include <lsp-plug.in/common/endian.h>
#include <lsp-plug.in/common/alloc.h>
#include <lsp-plug.in/common/debug.h>
#include <lsp-plug.in/stdlib/stdio.h>
#include <lsp-plug.in/lltl/parray.h>

#ifdef PLATFORM_WINDOWS
    #include <private/mm/ACMStream.h>
    #include <private/mm/MMIOReader.h>

    #include <windows.h>
    #include <mmsystem.h>
    #include <mmreg.h>
    #include <msacm.h>
#endif /* PLATFORM_WINDOWS */

#ifdef USE_LIBSNDFILE
    #if (__SIZEOF_INT__ == 4)
        #define AFS_S32_CPU     mm::SFMT_S32_CPU
    #endif

    #if (__SIZEOF_SHORT__ == 2)
        #define AFS_S16_CPU     mm::SFMT_S16_CPU
    #elif ((__SIZEOF_SHORT__ == 4) && (!defined(AFS_S32_CPU)))
        #define AFS_S32_CPU     mm::SFMT_S32_CPU
    #endif
#endif /* USE_LIBSNDFILE */

namespace lsp
{
    namespace mm
    {
    #ifndef USE_LIBSNDFILE
        struct WAVEFILE
        {
            MMIOReader         *pMMIO;
            ACMStream          *pACM;
            WAVEFORMATEX       *pFormat;
        };

        static ssize_t decode_sample_format(WAVEFORMATEX *wfe)
        {
            if (wfe->wFormatTag == WAVE_FORMAT_IEEE_FLOAT)
                return SFMT_F32_LE;

            if (wfe->wFormatTag == WAVE_FORMAT_PCM)
            {
                switch (wfe->wBitsPerSample)
                {
                    case 8: return SFMT_U8_CPU;
                    case 16: return SFMT_S16_LE;
                    case 24: return SFMT_S24_LE;
                    case 32: return SFMT_S32_LE;
                    default:
                        break;
                }
            }

            return -1;
        }
    #endif /* USE_LIBSNDFILE */
        
        InAudioFileStream::InAudioFileStream()
        {
            hHandle     = NULL;
            bSeekable   = false;
        }
        
        InAudioFileStream::~InAudioFileStream()
        {
            IInAudioStream::close();
            do_close();
        }

        status_t InAudioFileStream::do_close()
        {
            status_t res    = close_handle(hHandle);

            hHandle         = NULL;
            bSeekable       = false;
            nOffset         = -1;       // Mark as closed

            return set_error(res);
        }

        status_t InAudioFileStream::close_handle(handle_t *h)
        {
            if (h == NULL)
                return STATUS_OK;

        #ifdef USE_LIBSNDFILE
            int res     = sf_close(h);
            return (res == 0) ? STATUS_OK : STATUS_IO_ERROR;
        #else
            status_t res    = STATUS_OK;
            if (h->pACM != NULL)
            {
                res         = update_status(res, h->pACM->close());
                delete h->pACM;
                h->pACM     = NULL;
            }

            if (h->pMMIO != NULL)
            {
                res         = update_status(res, h->pMMIO->close());
                delete h->pMMIO;
                h->pMMIO    = NULL;
            }

            h->pFormat  = NULL;     // Freed by owner

            return res;
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
    #else
        ssize_t InAudioFileStream::read_acm_convert(void *dst, size_t nframes, size_t fmt)
        {
            size_t nread    = 0;
            void *sptr;
            uint8_t *dptr   = static_cast<uint8_t *>(dst);
            size_t fsize    = sformat_size_of(sFormat.format) * sFormat.channels;
            nframes        *= fsize;
            bool eof        = false;

            while (nread < nframes)
            {
                // Perform pull
                size_t can_pull     = nframes - nread;
                ssize_t count       = hHandle->pACM->pull(&sptr, can_pull, eof);
                if (count > 0)
                {
                    // Copy data to dptr
                    ::memcpy(dptr, sptr, count);

                    // Update pointers and repeat pull
                    dptr               += count;
                    nread              += count;
                    continue;
                }
                else if (count < 0)
                {
                    if (nread > 0)
                        break;// Analyze result
                    set_error(-count);
                    return count;
                }

                // Perform push if possible
                count               = hHandle->pACM->push(&sptr);
                if (count < 0)
                {
                    if (nread > 0)
                        break;
                    return -set_error(STATUS_UNKNOWN_ERR);
                }

                // Read data into buffer block and commit new position
                count               = hHandle->pMMIO->read(sptr, count);
                if (count < 0)
                {
                    if (count == -STATUS_EOF)
                        eof = true;
                    else if (nread > 0)
                        break;
                    set_error(-count);
                    return count;
                }

                hHandle->pACM->commit(count);
            }

            return nread / fsize;
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
            status_t res        = STATUS_OK;

            // Allocate handle
            handle_t *h     = static_cast<handle_t *>(malloc(sizeof(handle_t)));
            if (h == NULL)
                return set_error(STATUS_NO_MEM);
            lsp_finally { close_handle(h);  };

            // Try to load data using MMIO
            h->pMMIO        = new MMIOReader();
            if (h->pMMIO == NULL)
                return -set_error(STATUS_NO_MEM);
            if ((res = h->pMMIO->open(path)) != STATUS_OK)
                return set_error(res);

            // Analyze format
            WAVEFORMATEX *wfe   = h->pMMIO->format();
            if ((wfe->wFormatTag == WAVE_FORMAT_PCM) || (wfe->wFormatTag == WAVE_FORMAT_IEEE_FLOAT))
            {
                ssize_t fmt         = decode_sample_format(wfe);
                if (fmt <= 0)
                    return set_error(STATUS_UNSUPPORTED_FORMAT);

                // All is OK, use reading of PCM samples
                h->pACM             = NULL;
                h->pFormat          = wfe;

                sFormat.srate       = wfe->nSamplesPerSec;
                sFormat.channels    = wfe->nChannels;
                sFormat.frames      = h->pMMIO->frames();
                sFormat.format      = fmt;

                nOffset             = 0;
                bSeekable           = h->pMMIO->seekable();

                // Commit handle and return
                lsp::swap(hHandle, h);
                return set_error(STATUS_OK);
            }

            // Create ACM stream
            ACMStream *acm      = new ACMStream();
            if (acm == NULL)
                return set_error(STATUS_NO_MEM);
            if ((res = acm->read_pcm(wfe)) != STATUS_OK)
                return set_error(res);

            // Detect format
            wfe                 = acm->out_format();
            ssize_t fmt         = decode_sample_format(wfe);
            if (fmt <= 0)
                return set_error(STATUS_UNSUPPORTED_FORMAT);

            // All is ok
            h->pFormat          = acm->out_format();

            sFormat.srate       = wfe->nSamplesPerSec;
            sFormat.channels    = wfe->nChannels;
            sFormat.frames      = h->pMMIO->frames();
            sFormat.format      = fmt;

            nOffset             = 0;
            bSeekable           = false;

            // Commit handle and return
            lsp::swap(hHandle, h);
            return set_error(STATUS_OK);
        #endif /* USE_LIBSNDFILE */
        }

        status_t InAudioFileStream::close()
        {
            IInAudioStream::close();
            return do_close();
        }

        size_t InAudioFileStream::select_format(size_t fmt)
        {
        #ifdef USE_LIBSNDFILE
            // libsndfile allows one to do some sample conversions internally
            // we trust it more than our own sample converison routines
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
        #else
            // We always do sample conversion for PCM stream
            return sFormat.format;
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
        #else
            if (hHandle->pMMIO != NULL)
            {
                if (hHandle->pACM != NULL)
                    return read_acm_convert(dst, nframes, fmt);

                size_t fsize    = sformat_size_of(sFormat.format) * sFormat.channels;
                ssize_t nread   = hHandle->pMMIO->read(dst, fsize * nframes);
                return (nread < 0) ? nread : nread / fsize;
            }

            return -STATUS_NOT_SUPPORTED;
        #endif /* USE_LIBSNDFILE */
        }

        wssize_t InAudioFileStream::skip(wsize_t nframes)
        {
            if (is_closed())
                return -set_error(STATUS_CLOSED);
            if (!bSeekable)
                return IInAudioStream::skip(nframes);

        #ifdef USE_LIBSNDFILE
            sf_count_t res = sf_seek(hHandle, nframes, SEEK_CUR);
            if (res >= 0)
            {
                nOffset    += nframes;
                set_error(STATUS_OK);
                return nframes;
            }

            return -set_error(decode_sf_error(hHandle));
        #else
            if (hHandle->pMMIO != NULL)
            {
                size_t fsize    = sformat_size_of(sFormat.format) * LE_TO_CPU(hHandle->pFormat->nChannels);
                wssize_t res    = hHandle->pMMIO->seek((nOffset + nframes) * fsize);
                if (res < 0)
                {
                    set_error(-res);
                    return res;
                }
                nOffset         = res / fsize;
                return set_error(STATUS_OK);
            }

            return -set_error(STATUS_NOT_SUPPORTED);
        #endif /* USE_LIBSNDFILE */
        }

        wssize_t InAudioFileStream::seek(wsize_t nframes)
        {
            if (is_closed())
                return -set_error(STATUS_CLOSED);
            if (!bSeekable)
                return IInAudioStream::seek(nframes);

        #ifdef USE_LIBSNDFILE
            sf_count_t res = sf_seek(hHandle, nframes, SEEK_SET);
            if (res >= 0)
            {
                nOffset     = nframes;
                set_error(STATUS_OK);
                return nframes;
            }

            return -set_error(decode_sf_error(hHandle));
        #else
            if (hHandle->pMMIO != NULL)
            {
                size_t fsize    = sformat_size_of(sFormat.format) * LE_TO_CPU(hHandle->pFormat->nChannels);
                wssize_t res    = hHandle->pMMIO->seek(nframes * fsize);
                if (res < 0)
                {
                    set_error(-res);
                    return res;
                }
                nOffset         = res / fsize;
                return set_error(STATUS_OK);
            }

            return -set_error(STATUS_NOT_SUPPORTED);
        #endif /* USE_LIBSNDFILE */
        }
    
    } /* namespace mm */
} /* namespace lsp */
