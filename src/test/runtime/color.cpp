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

    void test_parse_rgb()
    {
        printf("Testing parse_rgb...");

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
        printf("Testing parse_rgba...");

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
        printf("Testing parse_hsl...");

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
        printf("Testing parse_hsla...");

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

    UTEST_MAIN
    {
        test_parse_rgb();
        test_parse_rgba();
        test_parse_hsl();
        test_parse_hsla();
    }

UTEST_END

