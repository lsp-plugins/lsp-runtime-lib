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

#include <lsp-plug.in/fmt/xpm/xpm.h>
#include <lsp-plug.in/fmt/xpm/Parser.h>

#include <private/fmt/xpm/XPM1BuiltinParser.h>

namespace lsp
{
    namespace xpm
    {

        status_t make_xpm1(Parser **dst,
                size_t width,
                size_t height,
                size_t num_colors,
                size_t chars_per_pixel,
                const char * const * colors,
                const char * const * pixels)
        {
            if (dst == NULL)
                return STATUS_BAD_ARGUMENTS;

            header_t hdr;

            hdr.version             = XPM1;
            hdr.width               = width;
            hdr.height              = height;
            hdr.num_colors          = num_colors;
            hdr.chars_per_pixel     = chars_per_pixel;
            hdr.x_hotspot           = 0;
            hdr.y_hotspot           = 0;
            hdr.has_extensions      = false;

            XPM1BuiltinParser * parser = new XPM1BuiltinParser(hdr, colors, pixels);
            if (parser == NULL)
                return STATUS_NO_MEM;

            *dst                    = parser;

            return STATUS_OK;
        }

    } /* namespace xpm */
} /* namespace lsp */


