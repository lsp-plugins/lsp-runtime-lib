/*
 * types.cpp
 *
 *  Created on: 16 апр. 2020 г.
 *      Author: sadko
 */

#include <lsp-plug.in/mm/types.h>

namespace lsp
{
    namespace mm
    {
        size_t size_of(sformat_t fmt)
        {
            switch (fmt)
            {
                case SFMT_U8: case SFMT_S8: return sizeof(uint8_t);
                case SFMT_U16: case SFMT_S16: return sizeof(uint16_t);
                case SFMT_U24: case SFMT_S24: case SFMT_U32: case SFMT_S32:
                    return sizeof(uint32_t);
                case SFMT_U64: case SFMT_S64:
                    return sizeof(uint64_t);
                case SFMT_F32:
                    return sizeof(f32_t);
                case SFMT_F64:
                    return sizeof(f64_t);
                default:
                    break;
            }

            return 0;
        }
    }
}


