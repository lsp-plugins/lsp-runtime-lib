/*
 * InStream.h
 *
 *  Created on: 16 апр. 2020 г.
 *      Author: sadko
 */

#ifndef LSP_PLUG_IN_MM_IINAUDIOSTREAM_H_
#define LSP_PLUG_IN_MM_IINAUDIOSTREAM_H_

#include <lsp-plug.in/runtime/version.h>
#include <lsp-plug.in/common/status.h>
#include <lsp-plug.in/mm/types.h>

namespace lsp
{
    namespace mm
    {
        /**
         * Interface for input audio stream
         */
        class IInAudioStream
        {
            private:
                IInAudioStream & operator = (const IInAudioStream &);

            protected:
                wssize_t        nOffset;            // Offset from beginning, negative if closed
                status_t        nErrorCode;         // Last error code
                uint8_t        *pBuffer;            // Buffer for sample format conversion
                audio_stream_t  sFormat;            // Audio stream format

            protected:
                inline status_t     set_error(status_t error) { return nErrorCode = error; }

                virtual ssize_t     direct_read(void *dst, size_t nframes);

                virtual ssize_t     conv_read(void *dst, size_t nframes, sformat_t fmt);

            public:
                explicit IInAudioStream();
                virtual ~IInAudioStream();

            public:
                /** Get last I/O error code
                 *
                 * @return last I/O error code
                 */
                inline status_t     last_error() const  { return nErrorCode; };

                /**
                 * Obtain the information about audio stream
                 * @param dst
                 * @return
                 */
                status_t            info(audio_stream_t *dst);

                /**
                 * Close audio stream
                 * @return status of operations
                 */
                virtual status_t    close();

                /**
                 * Skip amount of frames
                 * @param nframes number of frames to skip
                 * @return number of frames skipped or negative status code
                 */
                virtual wssize_t    skip(wsize_t nframes);

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
                virtual ssize_t     read(uint8_t *dst, size_t nframes);
                virtual ssize_t     read(int8_t *dst, size_t nframes);
                virtual ssize_t     read(uint16_t *dst, size_t nframes);
                virtual ssize_t     read(int16_t *dst, size_t nframes);
                virtual ssize_t     read(uint32_t *dst, size_t nframes);
                virtual ssize_t     read(int32_t *dst, size_t nframes);
                virtual ssize_t     read(uint64_t *dst, size_t nframes);
                virtual ssize_t     read(int64_t *dst, size_t nframes);
                virtual ssize_t     read(f32_t *dst, size_t nframes);
                virtual ssize_t     read(f64_t *dst, size_t nframes);

                /**
                 * Strict type reads
                 * @param dst destination buffer to perform read
                 * @param nframe number of frames to read
                 */
                inline ssize_t      read_u8(void *dst, size_t nframes)      { return read(static_cast<uint8_t *>(dst), nframes);    }
                inline ssize_t      read_s8(void *dst, size_t nframes)      { return read(static_cast<int8_t *>(dst), nframes);     }
                inline ssize_t      read_u16(void *dst, size_t nframes)     { return read(static_cast<uint16_t *>(dst), nframes);   }
                inline ssize_t      read_s16(void *dst, size_t nframes)     { return read(static_cast<int16_t *>(dst), nframes);    }
                inline ssize_t      read_u32(void *dst, size_t nframes)     { return read(static_cast<uint32_t *>(dst), nframes);   }
                inline ssize_t      read_s32(void *dst, size_t nframes)     { return read(static_cast<int32_t *>(dst), nframes);    }
                inline ssize_t      read_u64(void *dst, size_t nframes)     { return read(static_cast<uint64_t *>(dst), nframes);   }
                inline ssize_t      read_s64(void *dst, size_t nframes)     { return read(static_cast<int64_t *>(dst), nframes);    }
                inline ssize_t      read_f32(void *dst, size_t nframes)     { return read(static_cast<f32_t *>(dst), nframes);      }
                inline ssize_t      read_f64(void *dst, size_t nframes)     { return read(static_cast<f64_t *>(dst), nframes);      }
        };
    
    } /* namespace mm */
} /* namespace lsp */

#endif /* LSP_PLUG_IN_MM_IINAUDIOSTREAM_H_ */
