/*
 * Copyright (C) 2026 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2026 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-runtime-lib
 * Created on: 21 янв. 2026 г.
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

#ifndef PRIVATE_FMT_XPM_UTILS_H_
#define PRIVATE_FMT_XPM_UTILS_H_

#include <lsp-plug.in/runtime/version.h>
#include <lsp-plug.in/fmt/xpm/ColorItem.h>

namespace lsp
{
    namespace xpm
    {

        static uint32_t hex_char(char c)
        {
            if ((c >= '0') && (c <= '9'))
                return c - '0';
            if ((c >= 'a') && (c <= 'f'))
                return c - 'a' + 10;
            if ((c >= 'A') && (c <= 'F'))
                return c - 'A' + 10;
            return 0xffffffff;
        }

        static bool is_space(char c)
        {
            switch (c)
            {
                case ' ':
                case '\n':
                case '\r':
                case '\t':
                case '\v':
                    return true;
                default:
                    break;
            }
            return false;
        }

        static const char *parse_color_item(ColorItem & v, const char *str, const char *end)
        {
            if (str >= end)
                return NULL;

            size_t len;
            const char * const begin = str;

            if (*str == '#')
            {
                // Parse hexadecimal value
                ++str;
                uint64_t color = 0;
                while (str < end)
                {
                    if ((str - begin) >= 49)
                        return NULL;

                    const uint32_t hex = hex_char(*str);
                    if (hex > 0xff)
                        break;

                    ++str;
                    color       = (color << 4) | hex;
                }

                // Check that color is valid
                len = str - begin;
                if (len == 7)
                {
                    v.set_rgba32(uint32_t(color));
                    return str;
                }
                else if (len == 13)
                {
                    v.set_rgba64(uint64_t(color));
                    return str;
                }

                return NULL;
            }

            while (str < end)
            {
                if (is_space(*str))
                    break;
                ++str;
            }

            len = str - begin;
            if (len <= 0)
                return NULL;

            return (v.set_name(str, len)) ? str : NULL;
        }
    } /* namespace xpm */
} /* namespace lsp */

#endif /* PRIVATE_FMT_XPM_UTILS_H_ */
