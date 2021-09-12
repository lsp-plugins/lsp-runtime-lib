/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-runtime-lib
 * Created on: 7 мая 2020 г.
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
#include <lsp-plug.in/test-fw/helpers.h>
#include <lsp-plug.in/runtime/Color.h>

UTEST_BEGIN("runtime.runtime", color)

    status_t parse_rgb(const char *text, uint32_t value)
    {
        Color c;
        printf("  parsing value %s -> 0x%08lx\n", text, (long)value);
        status_t res = c.parse_rgb(text);
        if (res == STATUS_OK)
        {
            uint32_t v = c.rgba32();
            printf("  parsed: 0x%08lx\n", (long)v);
            res = (v == value) ? STATUS_OK : STATUS_INVALID_VALUE;
        }
        return res;
    }

    status_t parse_rgba(const char *text, uint32_t value)
    {
        Color c;
        printf("  parsing value %s -> %08lx\n", text, (long)value);
        status_t res = c.parse_rgba(text);
        if (res == STATUS_OK)
        {
            uint32_t v = c.rgba32();
            printf("  parsed: 0x%08lx\n", (long)v);
            res = (v == value) ? STATUS_OK : STATUS_INVALID_VALUE;
        }
        return res;
    }

    status_t parse_hsl(const char *text, uint32_t value)
    {
        Color c;
        printf("  parsing value %s -> %08lx\n", text, (long)value);
        status_t res = c.parse_hsl(text);
        if (res == STATUS_OK)
        {
            uint32_t v = c.hsla32();
            printf("  parsed: 0x%08lx\n", (long)v);
            res = (v == value) ? STATUS_OK : STATUS_INVALID_VALUE;
        }
        return res;
    }

    status_t parse_hsla(const char *text, uint32_t value)
    {
        Color c;
        printf("  parsing value %s -> %08lx\n", text, (long)value);
        status_t res = c.parse_hsla(text);
        if (res == STATUS_OK)
        {
            uint32_t v = c.hsla32();
            printf("  parsed: 0x%08lx\n", (long)v);
            res = (v == value) ? STATUS_OK : STATUS_INVALID_VALUE;
        }
        return res;
    }

    status_t parse(Color &c, const char *text)
    {
        printf("  parsing value %s\n", text);
        status_t res = c.parse(text);
        if (res != STATUS_OK)
            return res;

        if (c.is_rgb())
            printf("  parsed rgba: (%f, %f, %f, %f)\n", c.red(), c.green(), c.blue(), c.alpha());
        else if (c.is_hsl())
            printf("  parsed hsla: (%f, %f, %f, %f)\n", c.hsl_hue(), c.hsl_saturation(), c.hsl_lightness(), c.alpha());
        else if (c.is_xyz())
            printf("  parsed xyz: (%f, %f, %f, %f)\n", c.xyz_x(), c.xyz_y(), c.xyz_z(), c.alpha());
        return STATUS_OK;
    }

    bool match4(float a, float b, float c, float d, float ma, float mb, float mc, float md)
    {
        return (float_equals_adaptive(a, ma, 1e-4f)) &&
            (float_equals_adaptive(b, mb, 1e-4f)) &&
            (float_equals_adaptive(c, mc, 1e-4f)) &&
            (float_equals_adaptive(d, md, 1e-4f));
    }

    void test_parse_rgb()
    {
        printf("Testing parse_rgb...\n");

        UTEST_ASSERT(parse_rgb("#123", 0x00112233) == STATUS_OK);
        UTEST_ASSERT(parse_rgb("#112233", 0x00112233) == STATUS_OK);
        UTEST_ASSERT(parse_rgb("#111222333", 0x00112233) == STATUS_OK);
        UTEST_ASSERT(parse_rgb("#111122223333", 0x00112233) == STATUS_OK);
        UTEST_ASSERT(parse_rgb("   #112233\t", 0x00112233) == STATUS_OK);

        UTEST_ASSERT(parse_rgb("#0000", 0) == STATUS_BAD_FORMAT);
        UTEST_ASSERT(parse_rgb("112233", 0) == STATUS_BAD_FORMAT);
        UTEST_ASSERT(parse_rgb("@112233", 0) == STATUS_BAD_FORMAT);
        UTEST_ASSERT(parse_rgb("# 112233", 0) == STATUS_BAD_FORMAT);
        UTEST_ASSERT(parse_rgb("", 0) == STATUS_NO_DATA);
        UTEST_ASSERT(parse_rgb("   ", 0) == STATUS_NO_DATA);
    }

    void test_parse_rgba()
    {
        printf("Testing parse_rgba...\n");

        UTEST_ASSERT(parse_rgba("#1234", 0x11223344) == STATUS_OK);
        UTEST_ASSERT(parse_rgba("#11223344", 0x11223344) == STATUS_OK);
        UTEST_ASSERT(parse_rgba("#111222333444", 0x11223344) == STATUS_OK);
        UTEST_ASSERT(parse_rgba("#1111222233334444", 0x11223344) == STATUS_OK);
        UTEST_ASSERT(parse_rgba("   #11223344\t", 0x11223344) == STATUS_OK);

        UTEST_ASSERT(parse_rgba("#000", 0) == STATUS_BAD_FORMAT);
        UTEST_ASSERT(parse_rgba("11223344", 0) == STATUS_BAD_FORMAT);
        UTEST_ASSERT(parse_rgba("@11223344", 0) == STATUS_BAD_FORMAT);
        UTEST_ASSERT(parse_rgba("# 11223344", 0) == STATUS_BAD_FORMAT);
        UTEST_ASSERT(parse_rgba("", 0) == STATUS_NO_DATA);
        UTEST_ASSERT(parse_rgba("   ", 0) == STATUS_NO_DATA);
    }

    void test_parse_hsl()
    {
        printf("Testing parse_hsl...\n");

        UTEST_ASSERT(parse_hsl("@123", 0x00112233) == STATUS_OK);
        UTEST_ASSERT(parse_hsl("@112233", 0x00112233) == STATUS_OK);
        UTEST_ASSERT(parse_hsl("@111222333", 0x00112233) == STATUS_OK);
        UTEST_ASSERT(parse_hsl("@111122223333", 0x00112233) == STATUS_OK);
        UTEST_ASSERT(parse_hsl("   @112233\t", 0x00112233) == STATUS_OK);

        UTEST_ASSERT(parse_hsl("@0000", 0) == STATUS_BAD_FORMAT);
        UTEST_ASSERT(parse_hsl("112233", 0) == STATUS_BAD_FORMAT);
        UTEST_ASSERT(parse_hsl("#112233", 0) == STATUS_BAD_FORMAT);
        UTEST_ASSERT(parse_hsl("@ 112233", 0) == STATUS_BAD_FORMAT);
        UTEST_ASSERT(parse_hsl("", 0) == STATUS_NO_DATA);
        UTEST_ASSERT(parse_hsl("   ", 0) == STATUS_NO_DATA);
    }

    void test_parse_hsla()
    {
        printf("Testing parse_hsla...\n");

        UTEST_ASSERT(parse_hsla("@1234", 0x11223344) == STATUS_OK);
        UTEST_ASSERT(parse_hsla("@11223344", 0x11223344) == STATUS_OK);
        UTEST_ASSERT(parse_hsla("@111222333444", 0x11223344) == STATUS_OK);
        UTEST_ASSERT(parse_hsla("@1111222233334444", 0x11223344) == STATUS_OK);
        UTEST_ASSERT(parse_hsla("   @11223344\t", 0x11223344) == STATUS_OK);

        UTEST_ASSERT(parse_hsla("@000", 0) == STATUS_BAD_FORMAT);
        UTEST_ASSERT(parse_hsla("11223344", 0) == STATUS_BAD_FORMAT);
        UTEST_ASSERT(parse_hsla("#11223344", 0) == STATUS_BAD_FORMAT);
        UTEST_ASSERT(parse_hsla("@ 11223344", 0) == STATUS_BAD_FORMAT);
        UTEST_ASSERT(parse_hsla("", 0) == STATUS_NO_DATA);
        UTEST_ASSERT(parse_hsla("   ", 0) == STATUS_NO_DATA);
    }

    void test_parse()
    {
        printf("Testing parse_hsla...\n");
        Color c;

        // Match RGB parse
        UTEST_ASSERT(parse(c, "rgb(0.1,0.2,0.3)") == STATUS_OK);
        UTEST_ASSERT(c.is_rgb());
        UTEST_ASSERT(match4(c.red(), c.green(), c.blue(), c.alpha(), 0.1f, 0.2f, 0.3f, 0.0f));

        UTEST_ASSERT(parse(c, "RGBA(0.2,    0.3,    0.4,     0.5)") == STATUS_OK);
        UTEST_ASSERT(c.is_rgb());
        UTEST_ASSERT(match4(c.red(), c.green(), c.blue(), c.alpha(), 0.2f, 0.3f, 0.4f, 0.5f));

        // Match HSL parse
        UTEST_ASSERT(parse(c, "hsl   (72, 50, 150)   ") == STATUS_OK);
        UTEST_ASSERT(c.is_hsl());
        UTEST_ASSERT(match4(c.hsl_hue(), c.hsl_saturation(), c.hsl_lightness(), c.alpha(), 0.2f, 0.5f, 0.75f, 0.0f));

        UTEST_ASSERT(parse(c, "HSLA   (   144,    100,   50, 0.3   )   ") == STATUS_OK);
        UTEST_ASSERT(c.is_hsl());
        UTEST_ASSERT(match4(c.hsl_hue(), c.hsl_saturation(), c.hsl_lightness(), c.alpha(), 0.4f, 1.0f, 0.25f, 0.3f));

        // Match XYZ parse
        UTEST_ASSERT(parse(c, "xyz(10.0, 20.0, 30.5)") == STATUS_OK);
        UTEST_ASSERT(c.is_xyz());
        UTEST_ASSERT(match4(c.xyz_x(), c.xyz_y(), c.xyz_z(), c.alpha(), 10.0f, 20.0f, 30.5f, 0.0f));

        UTEST_ASSERT(parse(c, "XYZA(20.5, 30.0, 10.1, 0.7)") == STATUS_OK);
        UTEST_ASSERT(c.is_xyz());
        UTEST_ASSERT(match4(c.xyz_x(), c.xyz_y(), c.xyz_z(), c.alpha(), 20.5f, 30.0f, 10.1f, 0.7f));

        // Invalid parses
        UTEST_ASSERT(parse(c, "blablabla(1, 2, 3)") == STATUS_BAD_FORMAT);
        UTEST_ASSERT(parse(c, "rgb(1)") == STATUS_BAD_FORMAT);
        UTEST_ASSERT(parse(c, "rgb(1, 2)") == STATUS_BAD_FORMAT);
        UTEST_ASSERT(parse(c, "hsl(1, 2, 3, 4)") == STATUS_BAD_FORMAT);
        UTEST_ASSERT(parse(c, "hsla(1, 2, 3)") == STATUS_BAD_FORMAT);
        UTEST_ASSERT(parse(c, "xyz(") == STATUS_BAD_FORMAT);
        UTEST_ASSERT(parse(c, "xyz(  )") == STATUS_BAD_FORMAT);
        UTEST_ASSERT(parse(c, "xyz(1)") == STATUS_BAD_FORMAT);
        UTEST_ASSERT(parse(c, "xyz(1, 2, )") == STATUS_BAD_FORMAT);
        UTEST_ASSERT(parse(c, "xyz(1; 2; 3)") == STATUS_BAD_FORMAT);
        UTEST_ASSERT(parse(c, "xyza(1 2 3)") == STATUS_BAD_FORMAT);

        // Empty parses
        UTEST_ASSERT(parse(c, "") == STATUS_NO_DATA);
        UTEST_ASSERT(parse(c, "   ") == STATUS_NO_DATA);
        UTEST_ASSERT(parse(c, "\t\n\r") == STATUS_NO_DATA);
    }

    bool check_component(int c1, int c2, int delta = 5)
    {
        c1 -= c2;
        return (c1 >= -delta) && (c1 <= delta);
    }

    bool test_rgb2hsl(const char *value)
    {
        Color c1, c2;
        char buf[32];

        UTEST_ASSERT(c1.parse3(value) == STATUS_OK);
        printf("  col = %s\n", value);
        c1.format_hsl(buf, sizeof(buf), 2);
        printf("  hsl = %s\n", buf);
        c2.set_hsl24(c1.hsl24());
        c2.format_rgb(buf, sizeof(buf), 2);
        printf("  rgb = %s\n", buf);

        uint32_t v1 = c1.rgb24(), v2 = c2.rgb24();

        return
            check_component(v1 & 0xff, v2 & 0xff) &&
            check_component((v1 >> 8) & 0xff, (v2 >> 8) & 0xff) &&
            check_component((v1 >> 16) & 0xff, (v2 >> 16) & 0xff);
    }

    void test_convert_hsl()
    {
        printf("Testing RGB <-> HSL conversion...\n");

        UTEST_ASSERT(test_rgb2hsl("#ff0000"));
        UTEST_ASSERT(test_rgb2hsl("#00ff00"));
        UTEST_ASSERT(test_rgb2hsl("#0000ff"));
        UTEST_ASSERT(test_rgb2hsl("#ffff00"));
        UTEST_ASSERT(test_rgb2hsl("#ff00ff"));
        UTEST_ASSERT(test_rgb2hsl("#00ffff"));
        UTEST_ASSERT(test_rgb2hsl("#ffffff"));
        UTEST_ASSERT(test_rgb2hsl("#000000"));
        UTEST_ASSERT(test_rgb2hsl("#cccccc"));
    }

    bool test_rgb2xyz(const char *value, float x, float y, float z)
    {
        Color c1, c2, c3;
        char buf1[32], buf2[32];
        float cx[2], cy[2], cz[2];

        // Perform RGB -> XYZ conversion
        UTEST_ASSERT(c1.parse3(value) == STATUS_OK);
        printf("  col = %s\n", value);
        printf("  chk = (%f, %f, %f)\n", x, y, z);

        cx[0] = c1.xyz_x();
        cy[0] = c1.xyz_y();
        cz[0] = c1.xyz_z();
        c1.get_xyz(cx[1], cy[1], cz[1]);

        printf("  xyz = (%f, %f, %f)\n", cx[0], cy[0], cz[0]);
        if ((cx[0] != cx[1]) || (cy[0] != cy[1]) || (cz[0] != cz[1]))
            return false;
        if ((!float_equals_adaptive(x, cx[0], 1e-4f)) ||
            (!float_equals_adaptive(y, cy[0], 1e-4f)) ||
            (!float_equals_adaptive(z, cz[0], 1e-4f)))
            return false;

        // Perform XYZ -> RGB conversion
        c2.xyz_x(x);
        c2.xyz_y(y);
        c2.xyz_z(z);
        c2.format_rgb(buf1, sizeof(buf1), 2);
        printf("  rgb = %s\n", buf1);

        c3.set_xyz(x, y, z);
        c3.format_rgb(buf2, sizeof(buf2), 2);
        if (strcmp(buf1, buf2) != 0)
            return false;

        // Check components
        uint32_t v1 = c1.rgb24(), v2 = c2.rgb24();

        return
            check_component(v1 & 0xff, v2 & 0xff, 1) &&
            check_component((v1 >> 8) & 0xff, (v2 >> 8) & 0xff, 1) &&
            check_component((v1 >> 16) & 0xff, (v2 >> 16) & 0xff, 1);
    }

    void test_convert_xyz()
    {
        printf("Testing RGB <-> XYZ conversion...\n");

        UTEST_ASSERT(test_rgb2xyz("#000000", 0.0f, 0.0f, 0.0f));

        UTEST_ASSERT(test_rgb2xyz("#ff0000", 41.24f,  21.26f,   1.93f ));
        UTEST_ASSERT(test_rgb2xyz("#00ff00", 35.76f,  71.52f,  11.92f ));
        UTEST_ASSERT(test_rgb2xyz("#0000ff", 18.05f,   7.22f,  95.05f ));
        UTEST_ASSERT(test_rgb2xyz("#ffff00", 77.00f,  92.78f,  13.85f ));
        UTEST_ASSERT(test_rgb2xyz("#ff00ff", 59.29f,  28.48f,  96.98f ));
        UTEST_ASSERT(test_rgb2xyz("#00ffff", 53.81f,  78.74f, 106.97f ));
        UTEST_ASSERT(test_rgb2xyz("#ffffff", 95.05f, 100.00f, 108.90f ));

        UTEST_ASSERT(test_rgb2xyz("#880000",  10.15334f,   5.23424f,   0.47517f ));
        UTEST_ASSERT(test_rgb2xyz("#008800",   8.80416f,  17.60831f,   2.93472f ));
        UTEST_ASSERT(test_rgb2xyz("#000088",   4.44393f,   1.77757f,  23.40144f ));
        UTEST_ASSERT(test_rgb2xyz("#888800",  18.95750f,  22.84256f,   3.40989f ));
        UTEST_ASSERT(test_rgb2xyz("#880088",  14.59728f,   7.01181f,  23.87660f ));
        UTEST_ASSERT(test_rgb2xyz("#008888",  13.24809f,  19.38589f,  26.33616f ));
        UTEST_ASSERT(test_rgb2xyz("#888888",  23.40144f,  24.62013f,  26.81132f ));

        UTEST_ASSERT(test_rgb2xyz("#ffcc00",  62.83287f,  64.44573f,   9.12762f ));
        UTEST_ASSERT(test_rgb2xyz("#ff00cc",  52.13908f,  25.61963f,  59.32379f ));
        UTEST_ASSERT(test_rgb2xyz("#00ffcc",  46.65908f,  75.87963f,  69.31379f ));
        UTEST_ASSERT(test_rgb2xyz("#ccff00",  60.66184f,  84.35737f,  13.08539f ));
        UTEST_ASSERT(test_rgb2xyz("#cc00ff",  42.95184f,  20.05737f,  96.21539f ));
        UTEST_ASSERT(test_rgb2xyz("#00ccff",  39.64287f,  50.40573f, 102.24762f ));
    }

    UTEST_MAIN
    {
        test_parse_rgb();
        test_parse_rgba();
        test_parse_hsl();
        test_parse_hsla();
        test_parse();

        test_convert_hsl();
        test_convert_xyz();
    }

UTEST_END

