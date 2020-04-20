/*
 * OutStream.h
 *
 *  Created on: 16 апр. 2020 г.
 *      Author: sadko
 */

#ifndef LSP_PLUG_IN_MM_IOUTAUDIOSTREAM_H_
#define LSP_PLUG_IN_MM_IOUTAUDIOSTREAM_H_

#include <lsp-plug.in/runtime/version.h>
#include <lsp-plug.in/common/status.h>
#include <lsp-plug.in/mm/types.h>

namespace lsp
{
    namespace mm
    {
        class IOutAudioStream
        {
            private:
                IOutAudioStream & operator = (const IOutAudioStream &);

            public:
                static const size_t IO_BUF_SIZE         = 0x1000;

            protected:
                wssize_t            nOffset;            // Offset from beginning, negative if closed
                status_t            nErrorCode;         // Last error code
                uint8_t            *pBuffer;            // Buffer for sample format conversion
                size_t              nBufSize;           // Size of buffer
                audio_stream_t      sFormat;            // Audio stream format

            protected:
                void                do_close();

                inline status_t     set_error(status_t error) { return nErrorCode = error; }

                /**
                 * Ensure that internal buffer has enough capacity to keep specified number of bytes
                 * @param bytes number of bytes
                 * @return status of operation
                 */
                status_t            ensure_capacity(size_t bytes);

                /**
                 * Perform direct write of sample data
                 *
                 * @param src buffer with frame data.
                 * @param nframes number of frames to write
                 * @param fmt actual sample format that should be written
                 * @return number of frames actually written or negative error code
                 */
                virtual ssize_t     direct_write(const void *src, size_t nframes, size_t fmt);

                /**
                 * Perform write with conversion
                 * @param dst source buffer with frame data
                 * @param nframes number of frames to write
                 * @param fmt sample format
                 * @return status if operation
                 */
                virtual ssize_t     conv_write(const void *src, size_t nframes, size_t fmt);

                /**
                 * Select preferred sample format depending on the actual sample format
                 * @param rfmt requested format
                 * @return preferred sample format, by default 0 (unsupported format)
                 */
                virtual size_t      select_format(size_t rfmt);

            public:
                explicit IOutAudioStream();
                virtual ~IOutAudioStream();

                /** Get last I/O error code
                 *
                 * @return last I/O error code
                 */
                inline status_t     last_error() const          { return nErrorCode; };

                /**
                 * Obtain the information about audio stream
                 * @param dst destination to store the audio stream information
                 * @return status of operation
                 */
                status_t            info(audio_stream_t *dst) const;

                /**
                 * Get sample rate
                 * @return sample rate
                 */
                inline size_t       sample_rate() const         { return sFormat.srate;         }

                /**
                 * Get number of channels
                 * @return number of channels
                 */
                inline size_t       channels() const            { return sFormat.channels;      }

                /**
                 * Get overall duration
                 * @return overall number of frames available in audio stream,
                 * negative if number of frames can not be estimated
                 */
                inline wssize_t     length() const              { return sFormat.frames;        }

                /**
                 * Get sample format
                 * @return sample format
                 */
                inline size_t       format() const              { return sFormat.format;        }

                /**
                 * Flush audio stream
                 * @return status of operations
                 */
                virtual status_t    flush();

                /**
                 * Close audio stream
                 * @return status of operations
                 */
                virtual status_t    close();

                /**
                 * Get actual position in frames from beginning of the stream
                 * @return number of frames from beginning of the stream or negative error code
                 */
                virtual wssize_t    position();

                /**
                 * Try to seek stream at the specified position
                 * @return actual position in frames from beginnin of the stream
                 *   or negative error code on error (STATUS_NOT_SUPPORTED if stream
                 *   positioning is not supported)
                 */
                virtual wssize_t    seek(wsize_t nframes);

                /**
                 * Different reads
                 * @param dst destination buffer to perform read
                 * @param nframes number of frames to read
                 * @return number of frames actually read or negative status fo operation
                 */
                virtual ssize_t     write(const uint8_t *dst, size_t nframes);
                virtual ssize_t     write(const int8_t *dst, size_t nframes);
                virtual ssize_t     write(const uint16_t *dst, size_t nframes);
                virtual ssize_t     write(const int16_t *dst, size_t nframes);
                virtual ssize_t     write(const uint32_t *dst, size_t nframes);
                virtual ssize_t     write(const int32_t *dst, size_t nframes);
                virtual ssize_t     write(const f32_t *dst, size_t nframes);
                virtual ssize_t     write(const f64_t *dst, size_t nframes);

                /**
                 * Strict type reads
                 * @param dst destination buffer to perform read
                 * @param nframe number of frames to read
                 */
                inline ssize_t      write_u8(const void *dst, size_t nframes)     { return write(static_cast<const uint8_t *>(dst), nframes);   }
                inline ssize_t      write_s8(const void *dst, size_t nframes)     { return write(static_cast<const int8_t *>(dst), nframes);    }
                inline ssize_t      write_u16(const void *dst, size_t nframes)    { return write(static_cast<const uint16_t *>(dst), nframes);  }
                inline ssize_t      write_s16(const void *dst, size_t nframes)    { return write(static_cast<const int16_t *>(dst), nframes);   }
                inline ssize_t      write_u32(const void *dst, size_t nframes)    { return write(static_cast<const uint32_t *>(dst), nframes);  }
                inline ssize_t      write_s32(const void *dst, size_t nframes)    { return write(static_cast<const int32_t *>(dst), nframes);   }
                inline ssize_t      write_f32(const void *dst, size_t nframes)    { return write(static_cast<const f32_t *>(dst), nframes);     }
                inline ssize_t      write_f64(const void *dst, size_t nframes)    { return write(static_cast<const f64_t *>(dst), nframes);     }
        };
    
    } /* namespace mm */
} /* namespace lsp */

#endif /* LSP_PLUG_IN_MM_IOUTAUDIOSTREAM_H_ */
