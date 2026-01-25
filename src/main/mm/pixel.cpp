/*
 * Copyright (C) 2026 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2026 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-runtime-lib
 * Created on: 25 янв. 2026 г.
 *
 * lsp-runtime-lib is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * lsp-runtime-lib is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with lsp-runtime-lib. If not, see <https://www.gnu.org/licenses/>.
 */

#include <lsp-plug.in/mm/pixel.h>
#include <lsp-plug.in/stdlib/string.h>

namespace lsp
{
    namespace mm
    {
        size_t  bits_per_pixel(pixel_format_t format) noexcept
        {
            switch (format)
            {
                case PIXFMT_G1:         return 1;
                case PIXFMT_G2:         return 2;
                case PIXFMT_G4:         return 4;
                case PIXFMT_G8:         return 8;
                case PIXFMT_A1:         return 1;
                case PIXFMT_A2:         return 2;
                case PIXFMT_A4:         return 4;
                case PIXFMT_A8:         return 8;
                case PIXFMT_R5G5B5:     return 15;
                case PIXFMT_R5G5B5A1:   return 16;
                case PIXFMT_R5G6B5:     return 16;
                case PIXFMT_R8G8B8:     return 24;
                case PIXFMT_R8G8B8A8:   return 32;
                case PIXFMT_PR8G8B8A8:  return 32;
                default: break;
            }
            return 0;
        }

        size_t  device_bits_per_pixel(pixel_format_t format) noexcept
        {
            switch (format)
            {
                case PIXFMT_G1:         return 1;
                case PIXFMT_G2:         return 2;
                case PIXFMT_G4:         return 4;
                case PIXFMT_G8:         return 8;
                case PIXFMT_A1:         return 1;
                case PIXFMT_A2:         return 2;
                case PIXFMT_A4:         return 4;
                case PIXFMT_A8:         return 8;
                case PIXFMT_R5G5B5:     return 16;
                case PIXFMT_R5G5B5A1:   return 16;
                case PIXFMT_R5G6B5:     return 16;
                case PIXFMT_R8G8B8:     return 24;
                case PIXFMT_R8G8B8A8:   return 32;
                case PIXFMT_PR8G8B8A8:  return 32;
                default: break;
            }
            return 0;
        }

        bool is_exact_copy_of_pixel(pixel_format_t dst, pixel_format_t src) noexcept
        {
            if (dst == src)
                return true;
            switch (dst)
            {
                case PIXFMT_G1:         return src == PIXFMT_A1;
                case PIXFMT_G2:         return src == PIXFMT_A2;
                case PIXFMT_G4:         return src == PIXFMT_A4;
                case PIXFMT_G8:         return src == PIXFMT_A8;
                case PIXFMT_A1:         return src == PIXFMT_G1;
                case PIXFMT_A2:         return src == PIXFMT_G2;
                case PIXFMT_A4:         return src == PIXFMT_G4;
                case PIXFMT_A8:         return src == PIXFMT_G8;
                default:
                    break;
            }
            return false;
        }

        static const uint8_t b1tob2_table[] =
        {
            0x00, 0x03, 0x0c, 0x0f,
            0x30, 0x33, 0x3c, 0x3f,
            0xc0, 0xc3, 0xcc, 0xcf,
            0xf0, 0xf3, 0xfc, 0xff,
        };

        static const uint8_t b1tob4_table[] =
        {
            0x00, 0x0f, 0xf0, 0xff
        };

        static void convert_b1_to_b2(uint8_t *dst, const uint8_t *src, size_t count)
        {
            uint8_t v;
            for ( ; count >= 2; count -= 2)
            {
                v           = *(src++);
                dst[0]      = b1tob2_table[v & 0x0f];
                dst[1]      = b1tob2_table[v >> 4];
                dst        += 2;
            }
            if (count & 1)
            {
                v           = *src;
                *dst        = b1tob2_table[v & 0x0f];
            }
        }

        static void convert_b1_to_b4(uint8_t *dst, const uint8_t *src, size_t count)
        {
            uint8_t v;
            for ( ; count >= 4; count -= 4)
            {
                v           = *(src++);
                dst[0]      = b1tob4_table[v & 0x03];
                dst[1]      = b1tob4_table[(v >> 2) & 0x03];
                dst[2]      = b1tob4_table[(v >> 4) & 0x03];
                dst[3]      = b1tob4_table[(v >> 6) & 0x03];
                dst        += 4;
            }
            if (count > 0)
            {
                v           = *src;
                for (; count > 0; --count)
                {
                    *(dst++)    = b1tob4_table[v & 0x03];
                    v         >>= 2;
                }
            }
        }

        static void convert_b1_to_b8(uint8_t *dst, const uint8_t *src, size_t count)
        {
            uint8_t v;
            for ( ; count >= 8; count -= 8)
            {
                v           = *(src++);
                dst[0]      = (~(v & 1)) + 1;
                dst[1]      = (~((v >> 1) & 1) + 1);
                dst[2]      = (~((v >> 2) & 1) + 1);
                dst[3]      = (~((v >> 3) & 1) + 1);
                dst[4]      = (~((v >> 4) & 1) + 1);
                dst[5]      = (~((v >> 5) & 1) + 1);
                dst[6]      = (~((v >> 6) & 1) + 1);
                dst[7]      = (~((v >> 7) & 1) + 1);
                dst        += 8;
            }
            if (count > 0)
            {
                v           = *src;
                for (; count > 0; --count)
                {
                    *(dst++)    = (~(v & 1)) + 1;
                    v         >>= 1;
                }
            }
        }

        static void convert_b1_to_r5g5b5(uint8_t *dptr, const uint8_t *src, size_t count)
        {
            uint16_t v;
            uint16_t *dst = reinterpret_cast<uint16_t *>(dptr);

            for ( ; count >= 8; count -= 8)
            {
                v           = *(src++);
                dst[0]      = ((~(v & 1)) + 1) >> 1;
                dst[1]      = (~((v >> 1) & 1) + 1) >> 1;
                dst[2]      = (~((v >> 2) & 1) + 1) >> 1;
                dst[3]      = (~((v >> 3) & 1) + 1) >> 1;
                dst[4]      = (~((v >> 4) & 1) + 1) >> 1;
                dst[5]      = (~((v >> 5) & 1) + 1) >> 1;
                dst[6]      = (~((v >> 6) & 1) + 1) >> 1;
                dst[7]      = (~((v >> 7) & 1) + 1) >> 1;
                dst        += 8;
            }
            if (count > 0)
            {
                v           = *src;
                for (; count > 0; --count)
                {
                    *(dst++)    = ((~(v & 1)) + 1) >> 1;
                    v         >>= 1;
                }
            }
        }

        static void convert_a1_to_r5g5b5a1(uint8_t *dptr, const uint8_t *src, size_t count)
        {
            uint16_t v;
            uint16_t *dst = reinterpret_cast<uint16_t *>(dptr);

            for ( ; count >= 8; count -= 8)
            {
                v           = *(src++);
                dst[0]      = 0x7fff | (v << 15);
                dst[1]      = 0x7fff | ((v & 0x02) << 14);
                dst[2]      = 0x7fff | ((v & 0x04) << 13);
                dst[3]      = 0x7fff | ((v & 0x08) << 12);
                dst[4]      = 0x7fff | ((v & 0x10) << 11);
                dst[5]      = 0x7fff | ((v & 0x20) << 10);
                dst[6]      = 0x7fff | ((v & 0x40) << 9);
                dst[7]      = 0x7fff | ((v & 0x80) << 8);
                dst        += 8;
            }
            if (count > 0)
            {
                v           = *src;
                for (; count > 0; --count)
                {
                    *(dst++)    = 0x7fff | (v << 15);
                    v         >>= 1;
                }
            }
        }

        static void convert_b1_to_r5g6b5(uint8_t *dptr, const uint8_t *src, size_t count)
        {
            uint16_t v;
            uint16_t *dst = reinterpret_cast<uint16_t *>(dptr);

            for ( ; count >= 8; count -= 8)
            {
                v           = *(src++);
                dst[0]      = (~(v & 1)) + 1;
                dst[1]      = ~((v >> 1) & 1) + 1;
                dst[2]      = ~((v >> 2) & 1) + 1;
                dst[3]      = ~((v >> 3) & 1) + 1;
                dst[4]      = ~((v >> 4) & 1) + 1;
                dst[5]      = ~((v >> 5) & 1) + 1;
                dst[6]      = ~((v >> 6) & 1) + 1;
                dst[7]      = ~((v >> 7) & 1) + 1;
                dst        += 8;
            }
            if (count > 0)
            {
                v           = *src;
                for (; count > 0; --count)
                {
                    *(dst++)    = (~(v & 1)) + 1;
                    v         >>= 1;
                }
            }
        }

        static void convert_b1_to_r8g8b8(uint8_t *dst, const uint8_t *src, size_t count)
        {
            uint8_t v;
            const size_t bytes = (count + 7) >> 3;

            for (size_t i=0; i<bytes; ++i)
            {
                v           = *(src++);
                for (size_t j=0; j<8; ++j, v >>= 1)
                {
                    const uint8_t c = ~(v & 1) + 1;
                    dst[0]      = c;
                    dst[1]      = c;
                    dst[2]      = c;
                    dst        += 3;
                }
            }
        }

        static void convert_g1_to_r8g8b8a8(uint8_t *dst, const uint8_t *src, size_t count)
        {
            uint8_t v;
            const size_t bytes = (count + 7) >> 3;

            for (size_t i=0; i<bytes; ++i)
            {
                v           = *(src++);
                for (size_t j=0; j<8; ++j, v >>= 1)
                {
                    const uint8_t c = ~(v & 1) + 1;
                    dst[0]      = c;
                    dst[1]      = c;
                    dst[2]      = c;
                    dst[3]      = 0;
                    dst        += 4;
                }
            }
        }

        static void convert_a1_to_r8g8b8a8(uint8_t *dst, const uint8_t *src, size_t count)
        {
            uint8_t v;
            const size_t bytes = (count + 7) >> 3;

            for (size_t i=0; i<bytes; ++i)
            {
                v           = *(src++);
                for (size_t j=0; j<8; ++j, v >>= 1)
                {
                    dst[0]      = 0xff;
                    dst[1]      = 0xff;
                    dst[2]      = 0xff;
                    dst[3]      = ~(v & 1) + 1;

                    dst        += 4;
                }
            }
        }

        static void convert_b1_to_pr8g8b8a8(uint8_t *dptr, const uint8_t *src, size_t count)
        {
            uint32_t v;
            const size_t bytes  = (count + 7) >> 3;
            uint32_t *dst       = reinterpret_cast<uint32_t *>(dptr);

            for (size_t i=0; i<bytes; ++i)
            {
                v           = *(src++);
                for (size_t j=0; j<8; ++j, v >>= 1)
                {
                    *dst        = ~(v & 1) + 1;
                    dst        += 4;
                }
            }
        }

//        PIXFMT_R8G8B8,          /* 8 bits per each R, G and B component */
//        PIXFMT_R8G8B8A8,        /* 8 bits per each R, G and B component, 8 bits for alpha channel */
//        PIXFMT_PR8G8B8A8,       /* 8 bits per each R, G and B component, 8 bits for alpha channel, pre-multiplied alpha */

        static pixel_conversion_t pixel_convert_function_for_g1(pixel_format_t dst_fmt) noexcept
        {
            switch (dst_fmt)
            {
                case PIXFMT_G2:
                case PIXFMT_A2:
                    return convert_b1_to_b2;
                case PIXFMT_G4:
                case PIXFMT_A4:
                    return convert_b1_to_b4;
                case PIXFMT_G8:
                case PIXFMT_A8:
                    return convert_b1_to_b8;
                case PIXFMT_R5G5B5:
                case PIXFMT_R5G5B5A1:
                    return convert_b1_to_r5g5b5;
                case PIXFMT_R5G6B5:
                    return convert_b1_to_r5g6b5;
                case PIXFMT_R8G8B8:
                    return convert_b1_to_r8g8b8;
                case PIXFMT_R8G8B8A8:
                    return convert_g1_to_r8g8b8a8;
                case PIXFMT_PR8G8B8A8:
                    return convert_b1_to_pr8g8b8a8;

                default:
                    break;
            }
            return NULL;
        }

        static pixel_conversion_t pixel_convert_function_for_a1(pixel_format_t dst_fmt) noexcept
        {
            switch (dst_fmt)
            {
                case PIXFMT_G2:
                case PIXFMT_A2:
                    return convert_b1_to_b2;
                case PIXFMT_G4:
                case PIXFMT_A4:
                    return convert_b1_to_b4;
                case PIXFMT_G8:
                case PIXFMT_A8:
                    return convert_b1_to_b8;
                case PIXFMT_R5G5B5:
                    return convert_b1_to_r5g5b5;
                case PIXFMT_R5G5B5A1:
                    return convert_a1_to_r5g5b5a1;
                case PIXFMT_R5G6B5:
                    return convert_b1_to_r5g6b5;
                case PIXFMT_R8G8B8:
                    return convert_b1_to_r8g8b8;
                case PIXFMT_R8G8B8A8:
                    return convert_a1_to_r8g8b8a8;
                case PIXFMT_PR8G8B8A8:
                    return convert_b1_to_pr8g8b8a8;

                default:
                    break;
            }
            return NULL;
        }

        pixel_conversion_t pixel_convert_function(pixel_format_t dst_fmt, pixel_format_t src_fmt) noexcept
        {
            switch (src_fmt)
            {
                case PIXFMT_G1:
                    return pixel_convert_function_for_g1(dst_fmt);
                case PIXFMT_A1:
                    return pixel_convert_function_for_a1(dst_fmt);
                default:
                    break;
            }

            return NULL;
        }

        void convert_pixels(uint8_t *dst, const uint8_t *src, pixel_format_t dst_fmt, pixel_format_t src_fmt, size_t count) noexcept
        {
            const pixel_conversion_t func = pixel_convert_function(dst_fmt, src_fmt);
            if (func != NULL)
            {
                func(dst, src, count);
                return;
            }
            if (dst != src)
                memcpy(dst, src, device_bits_per_pixel(dst_fmt) * count);
        }

    } /* namespace mm */
} /* namespace lsp */
