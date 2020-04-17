/*
 * types.h
 *
 *  Created on: 16 апр. 2020 г.
 *      Author: sadko
 */

#ifndef LSP_PLUG_IN_MM_TYPES_H_
#define LSP_PLUG_IN_MM_TYPES_H_

#include <lsp-plug.in/runtime/version.h>
#include <lsp-plug.in/common/types.h>

namespace lsp
{
    namespace mm
    {
        /**
         * Sample format
         */
        enum sformat_t
        {
            SFMT_NONE,

            SFMT_U8         = 0 << 2,
            SFMT_S8         = 1 << 2,
            SFMT_U16        = 2 << 2,
            SFMT_S16        = 3 << 2,
            SFMT_U24        = 4 << 2,
            SFMT_S24        = 5 << 2,
            SFMT_U32        = 6 << 2,
            SFMT_S32        = 7 << 2,
            SFMT_F32        = 8 << 2,
            SFMT_F64        = 9 << 2,

            SFMT_DFL        = 0,
            SFMT_LE         = 1,
            SFMT_BE         = 2,
            SFMT_EMASK      = 0x03,
            SFMT_CPU        = __IF_LEBE(SFMT_LE, SFMT_BE)
        };

        typedef float       f32_t;
        typedef double      f64_t;

        typedef struct audio_stream_t
        {
            size_t      srate;          // Sample rate
            size_t      channels;       // Number of channels
            wssize_t    frames;         // Number of frames
            size_t      format;         // Sample format
        } audio_stream_t;

        inline size_t   sformat_endian(size_t fmt)        { return fmt & SFMT_EMASK;        }
        inline size_t   sformat_format(size_t fmt)        { return fmt & (~SFMT_EMASK);     }
        int             sformat_sign(size_t fmt);
        ssize_t         sformat_signed(size_t fmt);
        ssize_t         sformat_unsigned(size_t fmt);
        size_t          sformat_size_of(size_t fmt);
    }
}

#endif /* LSP_PLUG_IN_MM_TYPES_H_ */
