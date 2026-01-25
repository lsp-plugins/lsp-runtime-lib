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

        static void convert_b1_to_b2(uint8_t *dst, const uint8_t *src, size_t count)
        {
            uint8_t v;
            count   = (count + 3) >> 2; // Number of 4-bit blocks
            for ( ; count >= 2; count -= 2)
            {
                v           = *(src++);
                dst[0]      = b1tob2_table[v & 0x0f];
                dst[1]      = b1tob2_table[v >> 4];
                dst        += 2;
            }
            if (count <= 0)
                return;

            v           = *src;
            *dst        = b1tob2_table[v & 0x0f];
        }

        static const uint8_t b1tob4_table[] =
        {
            0x00, 0x0f, 0xf0, 0xff
        };

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
            if (count <= 0)
                return;

            v           = *(src++);
            for ( ; count > 0; --count)
            {
                dst[0]      = b1tob4_table[v & 0x03];
                v         >>= 2;
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
            if (count <= 0)
                return;

            v           = *src;
            for (; count > 0; --count)
            {
                *(dst++)    = (~(v & 1)) + 1;
                v         >>= 1;
            }
        }

        static void convert_b1_to_r8g8b8(uint8_t *dst, const uint8_t *src, size_t count)
        {
            uint8_t v, c;
            for (; count >= 8; count -= 8)
            {
                v           = *(src++);
                for (size_t j=0; j<8; ++j, v >>= 1)
                {
                    c           = ~(v & 1) + 1;
                    dst[0]      = c;
                    dst[1]      = c;
                    dst[2]      = c;
                    dst        += 3;
                }
            }

            if (count <= 0)
                return;

            v           = *(src++);
            for ( ; count > 0 ; --count, v >>= 1)
            {
                c           = ~(v & 1) + 1;
                dst[0]      = c;
                dst[1]      = c;
                dst[2]      = c;
                dst        += 3;
            }
        }

        static void convert_g1_to_r8g8b8a8(uint8_t *dst, const uint8_t *src, size_t count)
        {
            uint8_t v, c;

            for (; count >= 8; count -= 8)
            {
                v           = *(src++);
                for (size_t j=0; j<8; ++j, v >>= 1)
                {
                    c           = ~(v & 1) + 1;
                    dst[0]      = c;
                    dst[1]      = c;
                    dst[2]      = c;
                    dst[3]      = 0;
                    dst        += 4;
                }
            }

            if (count <= 0)
                return;

            v           = *(src++);
            for ( ; count > 0 ; --count, v >>= 1)
            {
                c           = ~(v & 1) + 1;
                dst[0]      = c;
                dst[1]      = c;
                dst[2]      = c;
                dst[3]      = 0;
                dst        += 4;
            }
        }

        static void convert_a1_to_r8g8b8a8(uint8_t *dst, const uint8_t *src, size_t count)
        {
            uint8_t v;
            for (; count >= 8; count -= 8)
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

            if (count <= 0)
                return;

            v           = *(src++);
            for ( ; count > 0 ; --count, v >>= 1)
            {
                dst[0]      = 0xff;
                dst[1]      = 0xff;
                dst[2]      = 0xff;
                dst[3]      = ~(v & 1) + 1;
                dst        += 4;
            }
        }

        static void convert_b1_to_pr8g8b8a8(uint8_t *dptr, const uint8_t *src, size_t count)
        {
            uint32_t v;
            uint32_t *dst       = reinterpret_cast<uint32_t *>(dptr);

            for (; count >= 8; count -= 8)
            {
                v           = *(src++);
                for (size_t j=0; j<8; ++j, v >>= 1)
                    *(dst++)        = ~(v & 1) + 1;
            }

            if (count <= 0)
                return;

            v           = *(src++);
            for ( ; count > 0 ; --count, v >>= 1)
                *(dst++)        = ~(v & 1) + 1;
        }

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

        static void convert_b2_to_b1(uint8_t *dst, const uint8_t *src, size_t count)
        {
            uint16_t v;
            for ( ; count >= 8; count -= 8)
            {
                v           = src[0] | (uint16_t(src[1]) << 8);
                v           = ((v & 0x8888) >> 3) | ((v & 0x2222) >> 1);
                v           = ((v & 0x0303) | (v & 0x3030)) >> 2;
                *(dst++)    = uint8_t((v & 0x0f) | (v >> 4));
                src        += 2;
            }
            if (count <= 0)
                return;

            v           = src[0];
            if (count >= 4)
                v          |= (uint16_t(src[1]) << 8);

            uint16_t c  = 0;
            for (size_t i=0; i<count; ++i)
                c          |= (v >> (i*2 + 1)) << i;

            *dst        = uint8_t(c);
        }

        static const uint8_t b2tob4_table[] =
        {
            0x00, 0x03, 0x0c, 0x0f,
            0x30, 0x33, 0x3c, 0x3f,
            0xc0, 0xc3, 0xcc, 0xcf,
            0xf0, 0xf3, 0xfc, 0xff
        };

        static void convert_b2_to_b4(uint8_t *dst, const uint8_t *src, size_t count)
        {
            uint8_t v;
            count       = (count + 1) >> 1; // Number of 4-bit blocks
            for ( ; count >= 2; count -= 2)
            {
                v           = *(src++);
                dst[0]      = b2tob4_table[v & 0x0f];
                dst[1]      = b2tob4_table[v >> 4];
                dst        += 2;
            }
            if (count <= 0)
                return;

            v           = *src;
            *dst        = b1tob2_table[v & 0x0f];
        }

        static const uint8_t b2tob8_table[] =
        {
            0x00, 0x55, 0xaa, 0xff
        };

        static void convert_b2_to_b8(uint8_t *dst, const uint8_t *src, size_t count)
        {
            uint8_t v;
            for ( ; count >= 4; count -= 4)
            {
                v           = *(src++);
                dst[0]      = b2tob8_table[v & 0x03];
                dst[1]      = b2tob8_table[(v >> 2) & 0x03];
                dst[2]      = b2tob8_table[(v >> 4) & 0x03];
                dst[3]      = b2tob8_table[(v >> 6) & 0x03];
                dst        += 4;
            }
            if (count <= 0)
                return;

            v           = *(src++);
            for ( ; count > 0; --count)
            {
                dst[0]      = b2tob8_table[v & 0x03];
                v         >>= 2;
            }
        }

        static void convert_b2_to_r8b8g8(uint8_t *dst, const uint8_t *src, size_t count)
        {
            uint8_t v, c;
            for (; count >= 4; count -= 4)
            {
                v           = *(src++);
                for (size_t j=0; j<4; ++j, v >>= 2)
                {
                    c           = b2tob8_table[v & 0x03];
                    dst[0]      = c;
                    dst[1]      = c;
                    dst[2]      = c;
                    dst        += 3;
                }
            }

            if (count <= 0)
                return;

            v           = *(src++);
            for ( ; count > 0 ; --count, v >>= 2)
            {
                c           = b2tob8_table[v & 0x03];
                dst[0]      = c;
                dst[1]      = c;
                dst[2]      = c;
                dst        += 3;
            }
        }

        static void convert_g2_to_r8b8g8a8(uint8_t *dst, const uint8_t *src, size_t count)
        {
            uint8_t v, c;
            for (; count >= 4; count -= 4)
            {
                v           = *(src++);
                for (size_t j=0; j<4; ++j, v >>= 2)
                {
                    c           = b2tob8_table[v & 0x03];
                    dst[0]      = c;
                    dst[1]      = c;
                    dst[2]      = c;
                    dst[3]      = 0;
                    dst        += 4;
                }
            }

            if (count <= 0)
                return;

            v           = *(src++);
            for ( ; count > 0 ; --count, v >>= 2)
            {
                c           = b2tob8_table[v & 0x03];
                dst[0]      = c;
                dst[1]      = c;
                dst[2]      = c;
                dst[3]      = 0;
                dst        += 4;
            }
        }

        static void convert_a2_to_r8b8g8a8(uint8_t *dst, const uint8_t *src, size_t count)
        {
            uint8_t v;
            for (; count >= 4; count -= 4)
            {
                v           = *(src++);
                for (size_t j=0; j<4; ++j, v >>= 2)
                {
                    dst[0]      = 0xff;
                    dst[1]      = 0xff;
                    dst[2]      = 0xff;
                    dst[3]      = b2tob8_table[v & 0x03];
                    dst        += 4;
                }
            }

            if (count <= 0)
                return;

            v           = *(src++);
            for ( ; count > 0 ; --count, v >>= 2)
            {
                dst[0]      = 0xff;
                dst[1]      = 0xff;
                dst[2]      = 0xff;
                dst[3]      = b2tob8_table[v & 0x03];
                dst        += 4;
            }
        }

        static void convert_b2_to_pr8b8g8a8(uint8_t *dst, const uint8_t *src, size_t count)
        {
            uint8_t v, c;
            for (; count >= 4; count -= 4)
            {
                v           = *(src++);
                for (size_t j=0; j<4; ++j, v >>= 2)
                {
                    c           = b2tob8_table[v & 0x03];
                    dst[0]      = c;
                    dst[1]      = c;
                    dst[2]      = c;
                    dst[3]      = c;
                    dst        += 4;
                }
            }

            if (count <= 0)
                return;

            v           = *(src++);
            for ( ; count > 0 ; --count, v >>= 2)
            {
                c           = b2tob8_table[v & 0x03];
                dst[0]      = c;
                dst[1]      = c;
                dst[2]      = c;
                dst[3]      = c;
                dst        += 4;
            }
        }

        static pixel_conversion_t pixel_convert_function_for_g2(pixel_format_t dst_fmt) noexcept
        {
            switch (dst_fmt)
            {
                case PIXFMT_G2:
                case PIXFMT_A2:
                    return convert_b2_to_b1;
                case PIXFMT_G4:
                case PIXFMT_A4:
                    return convert_b2_to_b4;
                case PIXFMT_G8:
                case PIXFMT_A8:
                    return convert_b2_to_b8;
                case PIXFMT_R8G8B8:
                    return convert_b2_to_r8b8g8;
                case PIXFMT_R8G8B8A8:
                    return convert_g2_to_r8b8g8a8;
                case PIXFMT_PR8G8B8A8:
                    return convert_b2_to_pr8b8g8a8;

                default:
                    break;
            }
            return NULL;
        }

        static pixel_conversion_t pixel_convert_function_for_a2(pixel_format_t dst_fmt) noexcept
        {
            switch (dst_fmt)
            {
                case PIXFMT_G2:
                case PIXFMT_A2:
                    return convert_b2_to_b1;
                case PIXFMT_G4:
                case PIXFMT_A4:
                    return convert_b2_to_b4;
                case PIXFMT_G8:
                case PIXFMT_A8:
                    return convert_b2_to_b8;
                case PIXFMT_R8G8B8:
                    return convert_b2_to_r8b8g8;
                case PIXFMT_R8G8B8A8:
                    return convert_a2_to_r8b8g8a8;
                case PIXFMT_PR8G8B8A8:
                    return convert_b2_to_pr8b8g8a8;

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
                case PIXFMT_G2:
                    return pixel_convert_function_for_g2(dst_fmt);
                case PIXFMT_A2:
                    return pixel_convert_function_for_a2(dst_fmt);
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
