/*
 * IInAudioFileStream.cpp
 *
 *  Created on: 19 апр. 2020 г.
 *      Author: sadko
 */

#include <lsp-plug.in/mm/InAudioFileStream.h>

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
        
        InAudioFileStream::InAudioFileStream()
        {
        #ifdef USE_LIBSNDFILE
            hHandle     = NULL;
            bSeekable   = false;
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

            int res = sf_close(hHandle);

            hHandle     = NULL;
            bSeekable   = false;
            nOffset     = -1;       // Mark as closed

            return set_error((res == 0) ? STATUS_OK : STATUS_IO_ERROR);
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
