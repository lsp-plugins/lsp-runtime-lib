/*
 * InStream.cpp
 *
 *  Created on: 16 апр. 2020 г.
 *      Author: sadko
 */

#include <lsp-plug.in/mm/IInAudioStream.h>
#include <lsp-plug.in/mm/sample.h>
#include <stdlib.h>

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

            nOffset     = -1;   // Mark as closed
            return set_error(STATUS_OK);
        }

        wssize_t IInAudioStream::skip(wsize_t nframes)
        {
            // Perform direct read
            wssize_t nread = 0;
            while (nframes > 0)
            {
                // Perform direct read
                size_t direct_fmt   = -1;
                size_t to_read      = (nframes > IO_BUF_SIZE) ? IO_BUF_SIZE : nframes;
                ssize_t read        = direct_read(NULL, to_read, -1, &direct_fmt);
                if (read < 0)
                {
                    if (nread <= 0)
                        return read;
                    break;
                }

                // Update position
                nframes    -= read;
            }

            // Update statistics
            set_error(STATUS_OK);
            nOffset    += nread;
            return nread;
        }

        wssize_t IInAudioStream::position()
        {
            return (nOffset >= 0) ? nOffset : -set_error(STATUS_CLOSED);
        }

        wssize_t IInAudioStream::seek(wsize_t nframes)
        {
            if (nOffset < 0)
                return -set_error(STATUS_CLOSED);
            else if (nOffset > wssize_t(nframes))
                return -set_error(STATUS_NOT_SUPPORTED);

            return skip(nframes - nOffset);
        }

        ssize_t IInAudioStream::direct_read(void *dst, size_t nframes, size_t rfmt, size_t *afmt)
        {
            return -set_error(STATUS_NOT_IMPLEMENTED);
        }

        status_t IInAudioStream::ensure_capacity(size_t bytes)
        {
            // Not enough space in temporary buffer?
            if (nBufSize >= bytes)
                return STATUS_OK;

            // Perform buffer re-allocation
            uint8_t *buf    = static_cast<uint8_t *>(::realloc(pBuffer, bytes));
            if (buf == NULL)
                return set_error(STATUS_NO_MEM);
            pBuffer         = buf;

            return STATUS_OK;
        }

        ssize_t IInAudioStream::conv_read(void *dst, size_t nframes, size_t fmt)
        {
            if (nOffset < 0)
                return -set_error(STATUS_CLOSED);

            // Prepare pointers and remember frame size
            size_t fsize    = sformat_size_of(fmt) * sFormat.channels;
            if (fsize <= 0)
                return -set_error(STATUS_BAD_FORMAT);
            uint8_t *dptr   = static_cast<uint8_t *>(dst);
            size_t nread    = 0;

            // Perform direct read
            while (nframes > 0)
            {
                // Perform direct read
                size_t direct_fmt   = -1;
                size_t to_read      = (nframes > IO_BUF_SIZE) ? IO_BUF_SIZE : nframes;
                ssize_t read        = direct_read(dst, to_read, fmt, &direct_fmt);
                if (read < 0)
                {
                    if (nread <= 0)
                        return read;
                    break;
                }

                // Need to perform conversion?
                if (direct_fmt != fmt)
                {
                    // Data is stored in pBuffer which can be updated, perform sample conversion
                    if (!convert_samples(dptr, pBuffer, to_read * sFormat.channels, fmt, direct_fmt))
                        return -set_error(STATUS_UNSUPPORTED_FORMAT);
                }

                // Update position and pointers
                nframes    -= read;
                nread      += read;
                dptr       += fsize * read;
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
