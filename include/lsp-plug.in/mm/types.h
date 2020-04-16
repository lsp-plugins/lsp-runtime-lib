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

            SFMT_U8,
            SFMT_S8,
            SFMT_U16,
            SFMT_S16,
            SFMT_U24,
            SFMT_S24,
            SFMT_U32,
            SFMT_S32,
            SFMT_U64,
            SFMT_S64,
            SFMT_F32,
            SFMT_F64
        };

        typedef float       f32_t;
        typedef double      f64_t;

        typedef struct audio_stream_t
        {
            size_t      srate;          // Sample rate
            size_t      channels;       // Number of channels
            wsize_t     frames;         // Number of frames
            sformat_t   format;         // Sample format
        } audio_stream_t;

        size_t  size_of(sformat_t fmt);
    }
}

#endif /* LSP_PLUG_IN_MM_TYPES_H_ */
