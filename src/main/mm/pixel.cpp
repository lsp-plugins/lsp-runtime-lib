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

        pixel_conversion_t pixel_convert_function(pixel_format_t dst_fmt, pixel_format_t src_fmt) noexcept
        {
            // TODO
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
