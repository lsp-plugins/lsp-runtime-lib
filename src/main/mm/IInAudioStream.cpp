/*
 * InStream.cpp
 *
 *  Created on: 16 апр. 2020 г.
 *      Author: sadko
 */

#include <lsp-plug.in/mm/IInAudioStream.h>
#include <lsp-plug.in/mm/sample.h>
#include <stdlib.h>

#define LSP_MM_IO_BUF_SIZE      0x1000

namespace lsp
{
    namespace mm
    {
        IInAudioStream::IInAudioStream()
        {
            nOffset             = -1;
            pBuffer             = NULL;
            nErrorCode          = STATUS_CLOSED;
            sFormat.srate       = 0;
            sFormat.channels    = 0;
            sFormat.frames      = 0;
            sFormat.format      = SFMT_NONE;
        }
        
        IInAudioStream::~IInAudioStream()
        {
        }
    
        status_t IInAudioStream::info(audio_stream_t *dst)
        {
            if (dst == NULL)
                return set_error(STATUS_BAD_ARGUMENTS);
            *dst    = sFormat;
            return set_error(STATUS_OK);
        }

        status_t IInAudioStream::close()
        {
            // Free allocated buffer if present
            if (pBuffer != NULL)
            {
                ::free(pBuffer);
                pBuffer     = NULL;
            }

            return set_error(STATUS_OK);
        }

        wssize_t IInAudioStream::skip(wsize_t nframes)
        {
            return -set_error(STATUS_NOT_IMPLEMENTED);
        }

        wssize_t IInAudioStream::position()
        {
            return (nOffset >= 0) ? nOffset : -set_error(STATUS_CLOSED);
        }

        wssize_t IInAudioStream::seek(wsize_t nframes)
        {
            return -set_error(STATUS_NOT_IMPLEMENTED);
        }

        ssize_t IInAudioStream::direct_read(void *dst, size_t nframes)
        {
            return -set_error(STATUS_NOT_IMPLEMENTED);
        }

        ssize_t IInAudioStream::conv_read(void *dst, size_t nframes, sformat_t fmt)
        {
            if (nOffset < 0)
                return -set_error(STATUS_CLOSED);

            size_t fsize    = sizeof(sFormat.format) * sFormat.channels;
            if (fsize <= 0)
                return -set_error(STATUS_BAD_FORMAT);
            uint8_t *dptr   = static_cast<uint8_t *>(dst);
            size_t nread    = 0;

            // Sample format matches?
            if (fmt == sFormat.format)
            {
                // Perform direct read
                for (size_t left = nframes - nframes; left > 0; )
                {
                    ssize_t read = direct_read(dptr, left);
                    if (read < 0)
                    {
                        if (nread <= 0)
                            return read;
                    }

                    // Update position and pointers
                    left   -= read;
                    nread  += read;
                    dptr   += fsize;
                }
            }
            else
            {
                // Need to allocate temporary buffer?
                if (pBuffer == NULL)
                {
                    pBuffer = static_cast<uint8_t *>(::malloc(fsize * LSP_MM_IO_BUF_SIZE));
                    if (pBuffer == NULL)
                        return -set_error(STATUS_NO_MEM);
                }

                // Perform direct read
                for (size_t left = nframes - nframes; left > 0; )
                {
                    size_t to_read  = (left > LSP_MM_IO_BUF_SIZE) ? LSP_MM_IO_BUF_SIZE : left;
                    ssize_t read    = direct_read(pBuffer, to_read);
                    if (read < 0)
                    {
                        if (nread <= 0)
                            return read;
                    }

                    // Convert samples
                    convert_samples(dptr, pBuffer, read * sFormat.channels, fmt, sFormat.format);

                    // Update pointers
                    left   -= read;
                    nread  += read;
                    dptr   += fsize;
                }
            }

            // Update statistics
            set_error(STATUS_OK);
            nOffset    += nread;
            return nread;
        }

        ssize_t IInAudioStream::read(uint8_t *dst, size_t nframes)
        {
            return conv_read(dst, nframes, SFMT_U8);
        }

        ssize_t IInAudioStream::read(int8_t *dst, size_t nframes)
        {
            return conv_read(dst, nframes, SFMT_S8);
        }

        ssize_t IInAudioStream::read(uint16_t *dst, size_t nframes)
        {
            return conv_read(dst, nframes, SFMT_U16);
        }

        ssize_t IInAudioStream::read(int16_t *dst, size_t nframes)
        {
            return conv_read(dst, nframes, SFMT_S16);
        }

        ssize_t IInAudioStream::read(uint32_t *dst, size_t nframes)
        {
            return conv_read(dst, nframes, SFMT_U32);
        }

        ssize_t IInAudioStream::read(int32_t *dst, size_t nframes)
        {
            return conv_read(dst, nframes, SFMT_S32);
        }

        ssize_t IInAudioStream::read(uint64_t *dst, size_t nframes)
        {
            return conv_read(dst, nframes, SFMT_U64);
        }

        ssize_t IInAudioStream::read(int64_t *dst, size_t nframes)
        {
            return conv_read(dst, nframes, SFMT_S64);
        }

        ssize_t IInAudioStream::read(f32_t *dst, size_t nframes)
        {
            return conv_read(dst, nframes, SFMT_F32);
        }

        ssize_t IInAudioStream::read(f64_t *dst, size_t nframes)
        {
            return conv_read(dst, nframes, SFMT_F64);
        }

    } /* namespace mm */
} /* namespace lsp */
