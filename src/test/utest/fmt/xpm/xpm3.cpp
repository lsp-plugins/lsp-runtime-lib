/*
 * Copyright (C) 2026 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2026 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-runtime-lib
 * Created on: 23 янв. 2026 г.
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

#define XFACE_width 23
#define XFACE_height 22
#define XFACE_ncolors 6
#define XFACE_chars_per_pixel 2

static const char * const XFACE_data[] = {
    "23 22 6 2 11 12 XPMEXT",
    "   c red m white s light_color ",
    "Y  c green m black s lines_in_mix",
    "+  c yellow m white s lines_in_dark g grey ",
    "x  m black s dark_color g4 grey ",
    ".* c #beefcafebabe",
    "./ c #abcdef",
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
    "          x           x   x   x Y x   x   x Y ",
    "XPMEXT ext1 data1",
    "XPMEXT ext2",
    "data2_1",
    "data2_2",
    "XPMENDEXT"
};

UTEST_BEGIN("runtime.fmt.xpm", xpm3)

    void validate_icon(lsp::xpm::Parser *parser)
    {
        // Read and validate header
        UTEST_ASSERT(parser != NULL);
        xpm::header_t hdr;
        UTEST_ASSERT(parser->read_header(&hdr) == STATUS_OK);

        UTEST_ASSERT(hdr.version == xpm::VERSION_XPM3);
        UTEST_ASSERT(hdr.width == XFACE_width);
        UTEST_ASSERT(hdr.height == XFACE_height);
        UTEST_ASSERT(hdr.num_colors == XFACE_ncolors);
        UTEST_ASSERT(hdr.x_hotspot == 11);
        UTEST_ASSERT(hdr.y_hotspot == 12);
        UTEST_ASSERT(hdr.has_extensions);

        // Read colors
        lsp::xpm::Color c;

        UTEST_ASSERT(parser->read_color(&c) == STATUS_OK);
        UTEST_ASSERT(c.has_code("  "));
        UTEST_ASSERT(c.mono_visual().has_name("white"));
        UTEST_ASSERT(c.symbolic_visual().has_name("light_color"));
        UTEST_ASSERT(!c.gray4_visual().is_set());
        UTEST_ASSERT(!c.gray_visual().is_set());
        UTEST_ASSERT(c.color_visual().has_name("red"));

        UTEST_ASSERT(parser->read_color(&c) == STATUS_OK);
        UTEST_ASSERT(c.has_code("Y "));
        UTEST_ASSERT(c.mono_visual().has_name("black"));
        UTEST_ASSERT(c.symbolic_visual().has_name("lines_in_mix"));
        UTEST_ASSERT(!c.gray4_visual().is_set());
        UTEST_ASSERT(!c.gray_visual().is_set());
        UTEST_ASSERT(c.color_visual().has_name("green"));

        UTEST_ASSERT(parser->read_color(&c) == STATUS_OK);
        UTEST_ASSERT(c.has_code("+ "));
        UTEST_ASSERT(c.mono_visual().has_name("white"));
        UTEST_ASSERT(c.symbolic_visual().has_name("lines_in_dark"));
        UTEST_ASSERT(!c.gray4_visual().is_set());
        UTEST_ASSERT(c.gray_visual().has_name("grey"));
        UTEST_ASSERT(c.color_visual().has_name("yellow"));

        UTEST_ASSERT(parser->read_color(&c) == STATUS_OK);
        UTEST_ASSERT(c.has_code("x "));
        UTEST_ASSERT(c.mono_visual().has_name("black"));
        UTEST_ASSERT(c.symbolic_visual().has_name("dark_color"));
        UTEST_ASSERT(c.gray4_visual().has_name("grey"));
        UTEST_ASSERT(!c.gray_visual().is_set());
        UTEST_ASSERT(!c.color_visual().is_set());

        UTEST_ASSERT(parser->read_color(&c) == STATUS_OK);
        UTEST_ASSERT(c.has_code(".*"));
        UTEST_ASSERT(!c.mono_visual().is_set());
        UTEST_ASSERT(!c.symbolic_visual().is_set());
        UTEST_ASSERT(!c.gray4_visual().is_set());
        UTEST_ASSERT(!c.gray_visual().is_set());
        UTEST_ASSERT(c.color_visual().rgb48() == 0xbeefcafebabeull);
        UTEST_ASSERT(c.color_visual().rgb24() == 0x00becaba);

        UTEST_ASSERT(parser->read_color(&c) == STATUS_OK);
        UTEST_ASSERT(c.has_code("./"));
        UTEST_ASSERT(!c.mono_visual().is_set());
        UTEST_ASSERT(!c.symbolic_visual().is_set());
        UTEST_ASSERT(!c.gray4_visual().is_set());
        UTEST_ASSERT(!c.gray_visual().is_set());
        UTEST_ASSERT(c.color_visual().rgb24() == 0x00abcdef);
        UTEST_ASSERT(c.color_visual().rgb48() == 0xab00cd00ef00ull);

        UTEST_ASSERT(parser->read_color(&c) == STATUS_NOT_FOUND);

        // Read rows
        char *buf = new char[XFACE_width * XFACE_chars_per_pixel];
        UTEST_ASSERT(buf != NULL);
        lsp_finally { delete[] buf; };

        for (size_t i=0; i<XFACE_height; ++i)
        {
            UTEST_ASSERT(parser->read_line(buf) == STATUS_OK);
            UTEST_ASSERT(memcmp(buf, XFACE_data[i + XFACE_ncolors + 1], XFACE_width * XFACE_chars_per_pixel) == 0);
        }

        UTEST_ASSERT(parser->read_line(buf) == STATUS_NOT_FOUND);

        // Read extensions
        xpm::Extension ext;

        UTEST_ASSERT(parser->read_ext(&ext) == STATUS_OK);
        UTEST_ASSERT(ext.has_name("ext1"));
        UTEST_ASSERT(ext.rows() == 1);
        UTEST_ASSERT(ext.row(0) != NULL);
        UTEST_ASSERT(strcmp(ext.row(0), "data1") == 0);

        UTEST_ASSERT(parser->read_ext(&ext) == STATUS_OK);
        UTEST_ASSERT(ext.has_name("ext2"));
        UTEST_ASSERT(ext.rows() == 2);
        UTEST_ASSERT(ext.row(0) != NULL);
        UTEST_ASSERT(strcmp(ext.row(0), "data2_1") == 0);
        UTEST_ASSERT(ext.row(1) != NULL);
        UTEST_ASSERT(strcmp(ext.row(1), "data2_2") == 0);

        UTEST_ASSERT(parser->read_ext(&ext) == STATUS_NOT_FOUND);
    }

    void test_read_builtin()
    {
        lsp::xpm::Parser *parser = NULL;
        UTEST_ASSERT(
            lsp::xpm::make_xpm3(
                &parser,
                XFACE_data, sizeof(XFACE_data)/ sizeof(XFACE_data[0])) == STATUS_OK);

        validate_icon(parser);

        // Close and delete parser
        UTEST_ASSERT(parser->close() == STATUS_OK);
        delete parser;
    }

    void test_read_file()
    {
        io::Path path;
        UTEST_ASSERT(path.fmt("%s/fmt/xpm/xpm3.xpm", resources()) > 0);

        printf("Reading file %s\n", path.as_native());

        lsp::xpm::Parser *parser = NULL;
        UTEST_ASSERT(lsp::xpm::open(&parser, &path) == STATUS_OK);

        validate_icon(parser);

        // Close and delete parser
        UTEST_ASSERT(parser->close() == STATUS_OK);
        delete parser;
    }

    UTEST_MAIN
    {
        printf("Testing read from built-in structure ...\n");
        test_read_builtin();
        printf("Testing read from file...\n");
        test_read_file();
    }

UTEST_END


