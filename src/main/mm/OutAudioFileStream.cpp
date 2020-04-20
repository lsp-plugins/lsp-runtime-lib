/*
 * OutAudioFileStream.cpp
 *
 *  Created on: 20 апр. 2020 г.
 *      Author: sadko
 */

#include <lsp-plug.in/mm/OutAudioFileStream.h>

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
        
        OutAudioFileStream::OutAudioFileStream()
        {
        #ifdef USE_LIBSNDFILE
            nCodec      = 0;
            hHandle     = NULL;
            bSeekable   = false;
        #endif /* USE_LIBSNDFILE */
        }
        
        OutAudioFileStream::~OutAudioFileStream()
        {
            IOutAudioStream::close();
            close_handle();
        }

    #ifdef USE_LIBSNDFILE
        status_t OutAudioFileStream::decode_sf_error(SNDFILE *fd)
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

        bool OutAudioFileStream::select_sndfile_format(SF_INFO *info, audio_stream_t *fmt, size_t codec)
        {
            int res = 0;

            // Choose audio format
            switch (codec & AFMT_MASK)
            {
                case AFMT_WAV:          res |= SF_FORMAT_WAV; break;
                case AFMT_AIFF:         res |= SF_FORMAT_AIFF; break;
                case AFMT_AU:           res |= SF_FORMAT_AU; break;
                case AFMT_RAW:          res |= SF_FORMAT_RAW; break;
                case AFMT_PAF:          res |= SF_FORMAT_PAF; break;
                case AFMT_SVX:          res |= SF_FORMAT_SVX; break;
                case AFMT_NIST:         res |= SF_FORMAT_NIST; break;
                case AFMT_VOC:          res |= SF_FORMAT_VOC; break;
                case AFMT_IRCAM:        res |= SF_FORMAT_IRCAM; break;
                case AFMT_W64:          res |= SF_FORMAT_W64; break;
                case AFMT_MAT4:         res |= SF_FORMAT_MAT4; break;
                case AFMT_MAT5:         res |= SF_FORMAT_MAT5; break;
                case AFMT_PVF:          res |= SF_FORMAT_PVF; break;
                case AFMT_XI:           res |= SF_FORMAT_XI; break;
                case AFMT_HTK:          res |= SF_FORMAT_HTK; break;
                case AFMT_SDS:          res |= SF_FORMAT_SDS; break;
                case AFMT_AVR:          res |= SF_FORMAT_AVR; break;
                case AFMT_WAVEX:        res |= SF_FORMAT_WAVEX; break;
                case AFMT_SD2:          res |= SF_FORMAT_SD2; break;
                case AFMT_FLAC:         res |= SF_FORMAT_FLAC; break;
                case AFMT_CAF:          res |= SF_FORMAT_CAF; break;
                case AFMT_WVE:          res |= SF_FORMAT_WVE; break;
                case AFMT_OGG:          res |= SF_FORMAT_OGG; break;
                case AFMT_MPC2K:        res |= SF_FORMAT_MPC2K; break;
                case AFMT_RF64:         res |= SF_FORMAT_RF64; break;
                default:
                    return false;
            }

            // Choose codec
            switch (codec & CFMT_MASK)
            {
                case CFMT_PCM:
                {
                    // Set sample format
                    switch (sformat_format(fmt->format))
                    {
                        case SFMT_U8:   res |= SF_FORMAT_PCM_U8; break;
                        case SFMT_S8:   res |= SF_FORMAT_PCM_S8; break;
                        case SFMT_U16:
                        case SFMT_S16:
                            res |= SF_FORMAT_PCM_16;
                            break;
                        case SFMT_S24:
                        case SFMT_U24:
                            res |= SF_FORMAT_PCM_24;
                            break;
                        case SFMT_S32:
                        case SFMT_U32:
                            res |= SF_FORMAT_PCM_32;
                            break;

                        case SFMT_F32:  res |= SF_FORMAT_FLOAT; break;
                        case SFMT_F64:  res |= SF_FORMAT_DOUBLE; break;
                        default:
                            return false;
                    }
                }
                break;

                case CFMT_ULAW:         res |= SF_FORMAT_ULAW; break;
                case CFMT_ALAW:         res |= SF_FORMAT_ALAW; break;
                case CFMT_IMA_ADPCM:    res |= SF_FORMAT_IMA_ADPCM; break;
                case CFMT_MS_ADPCM:     res |= SF_FORMAT_MS_ADPCM; break;
                case CFMT_GSM610:       res |= SF_FORMAT_GSM610; break;
                case CFMT_VOX_ADPCM:    res |= SF_FORMAT_VOX_ADPCM; break;
                case CFMT_G721_32:      res |= SF_FORMAT_G721_32; break;
                case CFMT_G723_24:      res |= SF_FORMAT_G723_24; break;
                case CFMT_G723_40:      res |= SF_FORMAT_G723_40; break;
                case CFMT_DWVW_12:      res |= SF_FORMAT_DWVW_12; break;
                case CFMT_DWVW_16:      res |= SF_FORMAT_DWVW_16; break;
                case CFMT_DWVW_24:      res |= SF_FORMAT_DWVW_24; break;
                case CFMT_DWVW_N:       res |= SF_FORMAT_DWVW_N; break;
                case CFMT_DPCM_8:       res |= SF_FORMAT_DPCM_8; break;
                case CFMT_DPCM_16:      res |= SF_FORMAT_DPCM_16; break;
                case CFMT_VORBIS:       res |= SF_FORMAT_VORBIS; break;
                case CFMT_ALAC_16:      res |= SF_FORMAT_ALAC_16; break;
                case CFMT_ALAC_20:      res |= SF_FORMAT_ALAC_20; break;
                case CFMT_ALAC_24:      res |= SF_FORMAT_ALAC_24; break;
                case CFMT_ALAC_32:      res |= SF_FORMAT_ALAC_32; break;

                default:
                    return false;
            }

            // Choose endianess
            switch (sformat_endian(fmt->format))
            {
                case SFMT_DFL:          res |= SF_ENDIAN_FILE; break;
                case SFMT_LE:           res |= SF_ENDIAN_LITTLE; break;
                case SFMT_BE:           res |= SF_ENDIAN_BIG; break;
                default:
                    return false;
            }

            info->frames        = fmt->frames;
            info->samplerate    = fmt->srate;
            info->channels      = fmt->channels;
            info->format        = res;
            info->sections      = 0;
            info->seekable      = 0;

            return true;
        }
    #endif /* USE_LIBSNDFILE */

        status_t OutAudioFileStream::open(const char *path, const audio_stream_t *fmt, size_t codec)
        {
            if (path == NULL)
                return set_error(STATUS_BAD_ARGUMENTS);
            LSPString xpath;
            if (!xpath.set_utf8(path))
                return set_error(STATUS_NO_MEM);
            return open(&xpath, fmt, codec);
        }

        status_t OutAudioFileStream::open(const io::Path *path, const audio_stream_t *fmt, size_t codec)
        {
            if (path == NULL)
                return set_error(STATUS_BAD_ARGUMENTS);
            return open(path->as_string(), fmt, codec);
        }

        status_t OutAudioFileStream::open(const LSPString *path, const audio_stream_t *fmt, size_t codec)
        {
            if (!is_closed())
                return set_error(STATUS_OPENED);
            if (fmt == NULL)
                return set_error(STATUS_BAD_ARGUMENTS);

        #ifdef USE_LIBSNDFILE
            audio_stream_t tmp;
            SF_INFO info;
            tmp     = *fmt;

            if (!select_sndfile_format(&info, &tmp, codec))
                return set_error(STATUS_UNSUPPORTED_FORMAT);

            // Open handle
            SNDFILE *sf = sf_open(path->get_native(), SFM_WRITE, &info);
            if (sf == NULL)
                return -set_error(decode_sf_error(sf));

            // Commit new state
            sFormat             = tmp;
            hHandle             = sf;
            nOffset             = 0;
            bSeekable           = info.seekable;

            return set_error(STATUS_OK);
        #endif /* USE_LIBSNDFILE */
        }

        status_t OutAudioFileStream::close_handle()
        {
            if (hHandle == NULL)
                return STATUS_OK;

        #ifdef USE_LIBSNDFILE
            int res     = sf_close(hHandle);

            hHandle     = NULL;
            bSeekable   = false;
            nOffset     = -1;       // Mark as closed
            nCodec      = 0;

            return set_error((res == 0) ? STATUS_OK : STATUS_IO_ERROR);
        #endif /* USE_LIBSNDFILE */
        }

        status_t OutAudioFileStream::flush()
        {
            if (is_closed())
                return -set_error(STATUS_CLOSED);

        #ifdef USE_LIBSNDFILE
            sf_write_sync(hHandle);
            return set_error(STATUS_OK);
        #endif /* USE_LIBSNDFILE */
        }

        status_t OutAudioFileStream::close()
        {
            IOutAudioStream::close();
            return close_handle();
        }

        ssize_t OutAudioFileStream::direct_write(const void *src, size_t nframes, size_t fmt)
        {
        #ifdef USE_LIBSNDFILE
            sf_count_t count;
            status_t res;

            switch (sformat_format(fmt))
            {
                case SFMT_S32:
                    count   = sf_writef_int(hHandle, static_cast<const int *>(src), nframes);
                    break;

                case SFMT_S16:
                    count   = sf_writef_short(hHandle, static_cast<const short *>(src), nframes);
                    break;

                case SFMT_F32:
                    count   = sf_writef_float(hHandle, static_cast<const float *>(src), nframes);
                    break;

                case SFMT_F64:
                    count   = sf_writef_double(hHandle, static_cast<const double *>(src), nframes);
                    break;

                default: // Force SFMT_F32 sample format
                    count   = sf_writef_float(hHandle, static_cast<const float *>(src), nframes);
                    break;
            }

            // Is there data written?
            if (count > 0)
                return count;

            res = decode_sf_error(hHandle);
            return -((res == STATUS_OK) ? STATUS_EOF : res);
        #endif /* USE_LIBSNDFILE */
        }

        size_t OutAudioFileStream::select_format(size_t rfmt)
        {
        #ifdef USE_LIBSNDFILE
            switch (sformat_format(rfmt))
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

        wssize_t OutAudioFileStream::seek(wsize_t nframes)
        {
            if (is_closed())
                return -set_error(STATUS_CLOSED);

        #ifdef USE_LIBSNDFILE
            sf_count_t offset = sf_seek(hHandle, nframes, SF_SEEK_SET);
            if (offset < 0)
                return -set_error(decode_sf_error(hHandle));

            set_error(STATUS_OK);
            return nOffset = offset;
        #endif /* USE_LIBSNDFILE */
        }
    } /* namespace mm */
} /* namespace lsp */
