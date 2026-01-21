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

#include <lsp-plug.in/test-fw/utest.h>
#include <lsp-plug.in/fmt/xpm/xpm.h>
#include <lsp-plug.in/stdlib/string.h>

#define XFACE_format 1
#define XFACE_width 23
#define XFACE_height 22
#define XFACE_ncolors 4
#define XFACE_chars_per_pixel 2

static const char *XFACE_colors[] = {
    "  ", "#ff0000",
    "Y ", "#00ff00",
    "+ ", "#ffff00",
    "x ", "#ffffff"
};

static const char *XFACE_pixels[] = {
    "x   x   x x x   x   x x x x x x + x x x x x x ",
    "  x   x   x   x   x   x x x x x x x x x x x Y ",
    "x   x   x x x   x   x x x x x x + x x x x x x ",
    "  x   x   x   x   x   x x x x x x x x x x x Y ",
    "x   x   x x x   x   x x x x x x + x x x x x x ",
    "Y Y Y Y Y x Y Y Y Y Y + x + x + x + x + x + Y ",
    "x   x   x x x   x   x x x x x x + x x x x x x ",
    "  x   x   x   x   x   x x x x x x x x x x x Y ",
    "x   x   x x x   x   x x x x x x + x x x x x x ",
    "  x   x   x   x   x   x x x x x x x x x x x Y ",
    "x   x   x x x   x   x x x x x x + x x x x x x ",
    "          x           x   x   x Y x   x   x Y ",
    "          x             x   x   Y   x   x   x ",
    "          x           x   x   x Y x   x   x Y ",
    "          x             x   x   Y   x   x   x ",
    "          x           x   x   x Y x   x   x Y ",
    "x x x x x x x x x x x x x x x x x x x x x x x ",
    "          x           x   x   x Y x   x   x Y ",
    "          x             x   x   Y   x   x   x ",
    "          x           x   x   x Y x   x   x Y ",
    "          x             x   x   Y   x   x   x ",
    "          x           x   x   x Y x   x   x Y "
};


UTEST_BEGIN("runtime.fmt.xpm", xpm1)

    bool validate_color(const lsp::xpm::Color & c, const char *code, uint32_t rgba)
    {
        if (c.code() == NULL)
            return false;
        if (strcmp(c.code(), code) != 0)
            return false;

        if (!c.mono_visual().is_none())
            return false;
        if (!c.symbolic_visual().is_none())
            return false;
        if (!c.gray4_visual().is_none())
            return false;
        if (!c.gray_visual().is_none())
            return false;

        if (c.color_visual().is_none())
            return false;
        if (c.color_visual().is_wide())
            return false;
        if (c.color_visual().rgba32() != rgba)
            return false;
        if (c.color_visual().name() != NULL)
            return false;

        return true;
    }

    void validate_icon(lsp::xpm::Parser *parser)
    {
        // Read and validate header
        UTEST_ASSERT(parser != NULL);
        xpm::header_t hdr;
        UTEST_ASSERT(parser->read_header(&hdr) == STATUS_OK);

        UTEST_ASSERT(hdr.version == xpm::VERSION_XPM1);
        UTEST_ASSERT(hdr.width == XFACE_width);
        UTEST_ASSERT(hdr.height == XFACE_height);
        UTEST_ASSERT(hdr.num_colors == XFACE_ncolors);
        UTEST_ASSERT(hdr.x_hotspot == 0);
        UTEST_ASSERT(hdr.y_hotspot == 0);
        UTEST_ASSERT(!hdr.has_extensions);

        // Read colors
        lsp::xpm::Color c;
        UTEST_ASSERT(parser->read_color(&c) == STATUS_OK);
        UTEST_ASSERT(validate_color(c, "  ", 0xff0000));
        UTEST_ASSERT(parser->read_color(&c) == STATUS_OK);
        UTEST_ASSERT(validate_color(c, "Y ", 0x00ff00));
        UTEST_ASSERT(parser->read_color(&c) == STATUS_OK);
        UTEST_ASSERT(validate_color(c, "+ ", 0xffff00));
        UTEST_ASSERT(parser->read_color(&c) == STATUS_OK);
        UTEST_ASSERT(validate_color(c, "x ", 0xffffff));
        UTEST_ASSERT(parser->read_color(&c) == STATUS_NOT_FOUND);

        // Read rows
        char *buf = new char[XFACE_width * XFACE_chars_per_pixel];
        UTEST_ASSERT(buf != NULL);
        lsp_finally { delete[] buf; };

        for (size_t i=0; i<XFACE_height; ++i)
        {
            UTEST_ASSERT(parser->read_line(buf) == STATUS_OK);
            UTEST_ASSERT(memcmp(buf, XFACE_pixels[i], XFACE_width * XFACE_chars_per_pixel) == 0);
        }

        UTEST_ASSERT(parser->read_line(buf) == STATUS_NOT_FOUND);

        // Read extensions
        size_t count = XFACE_width * XFACE_chars_per_pixel;
        UTEST_ASSERT(parser->read_ext(buf, &count) == STATUS_NOT_FOUND);
    }

    void test_read_builtin()
    {
        lsp::xpm::Parser *parser = NULL;
        UTEST_ASSERT(
            lsp::xpm::make_xpm1(
                &parser,
                XFACE_width, XFACE_height,
                XFACE_ncolors, XFACE_chars_per_pixel,
                XFACE_colors, XFACE_pixels) == STATUS_OK);

        validate_icon(parser);
    }

    void test_read_file()
    {
    }

    UTEST_MAIN
    {
        printf("Testing read from built-in structure ...\n");
        test_read_builtin();
        printf("Testing read from file...\n");
        test_read_file();
    }

UTEST_END



