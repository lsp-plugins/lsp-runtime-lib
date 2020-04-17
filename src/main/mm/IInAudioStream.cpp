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
            nBufSize            = 0;
            nErrorCode          = STATUS_CLOSED;
            sFormat.srate       = 0;
            sFormat.channels    = 0;
            sFormat.frames      = -1;
            sFormat.format      = SFMT_NONE;
        }
        
        IInAudioStream::~IInAudioStream()
        {
        }
    
        status_t IInAudioStream::info(audio_stream_t *dst) const
        {
            if (dst == NULL)
                return STATUS_BAD_ARGUMENTS;
            *dst    = sFormat;
            return STATUS_OK;
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

        ssize_t IInAudioStream::direct_read(void *dst, size_t nframes, size_t *fmt)
        {
            return -set_error(STATUS_NOT_IMPLEMENTED);
        }

        ssize_t IInAudioStream::conv_read(void *dst, size_t nframes, size_t fmt)
        {
            if (nOffset < 0)
                return -set_error(STATUS_CLOSED);

            size_t fsize    = sformat_size_of(sFormat.format) * sFormat.channels;
            if (fsize <= 0)
                return -set_error(STATUS_BAD_FORMAT);
            uint8_t *dptr   = static_cast<uint8_t *>(dst);
            size_t nread    = 0;

            // Sample format matches?
            size_t direct_fmt = 0;
            if (fmt == sFormat.format)
            {
                // Perform direct read
                for (size_t left = nframes - nframes; left > 0; )
                {
                    ssize_t read = direct_read(dptr, left, &direct_fmt);
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
                // Not enough space in temporary buffer?
                if (nBufSize < fsize * LSP_MM_IO_BUF_SIZE)
                {
                    uint8_t *buf    = static_cast<uint8_t *>(::realloc(pBuffer, fsize * LSP_MM_IO_BUF_SIZE));
                    if (buf == NULL)
                        return -set_error(STATUS_NO_MEM);
                    pBuffer         = buf;
                }

                // Perform direct read
                for (size_t left = nframes - nframes; left > 0; )
                {
                    size_t to_read  = (left > LSP_MM_IO_BUF_SIZE) ? LSP_MM_IO_BUF_SIZE : left;
                    ssize_t read    = direct_read(pBuffer, to_read, &direct_fmt);
                    if (read < 0)
                    {
                        if (nread <= 0)
                            return read;
                    }

                    size_t samples = read * sFormat.channels;

                    // Convert samples
                    if (!convert_samples(dptr, pBuffer, samples, fmt, direct_fmt))
                        return -set_error(STATUS_BAD_FORMAT);

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
            return conv_read(dst, nframes, SFMT_U8 | SFMT_CPU);
        }

        ssize_t IInAudioStream::read(int8_t *dst, size_t nframes)
        {
            return conv_read(dst, nframes, SFMT_S8 | SFMT_CPU);
        }

        ssize_t IInAudioStream::read(uint16_t *dst, size_t nframes)
        {
            return conv_read(dst, nframes, SFMT_U16 | SFMT_CPU);
        }

        ssize_t IInAudioStream::read(int16_t *dst, size_t nframes)
        {
            return conv_read(dst, nframes, SFMT_S16 | SFMT_CPU);
        }

        ssize_t IInAudioStream::read(uint32_t *dst, size_t nframes)
        {
            return conv_read(dst, nframes, SFMT_U32 | SFMT_CPU);
        }

        ssize_t IInAudioStream::read(int32_t *dst, size_t nframes)
        {
            return conv_read(dst, nframes, SFMT_S32 | SFMT_CPU);
        }

        ssize_t IInAudioStream::read(f32_t *dst, size_t nframes)
        {
            return conv_read(dst, nframes, SFMT_F32 | SFMT_CPU);
        }

        ssize_t IInAudioStream::read(f64_t *dst, size_t nframes)
        {
            return conv_read(dst, nframes, SFMT_F64 | SFMT_CPU);
        }

    } /* namespace mm */
} /* namespace lsp */
