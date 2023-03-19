/*
 * Copyright (C) 2023 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2023 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-runtime-lib
 * Created on: 2 мар. 2023 г.
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

#include <lsp-plug.in/fmt/sfz/parse.h>
#include <lsp-plug.in/test-fw/helpers.h>
#include <lsp-plug.in/test-fw/utest.h>

UTEST_BEGIN("runtime.fmt.sfz", parse)

    void check_bool(const char *text, bool value)
    {
        bool tmp;
        printf("check_bool %s\n", text);
        UTEST_ASSERT_MSG(
            sfz::parse_bool(&tmp, text) == STATUS_OK,
            "Failed parse boolean value text='%s'",
            text);
        UTEST_ASSERT_MSG(
            tmp == value,
            "Invalid boolean value text='%s', value=%s, expected=%s",
            text, (tmp) ? "true" : "false", (value) ? "true" : "false");
    }

    void check_int(const char *text, ssize_t value)
    {
        ssize_t tmp;
        printf("check_int %s\n", text);
        UTEST_ASSERT_MSG(
            sfz::parse_int(&tmp, text) == STATUS_OK,
            "Failed parse integer value text='%s'",
            text);
        UTEST_ASSERT_MSG(
            tmp == value,
            "Invalid integer value text='%s', value=%d, expected=%d",
            text, int(tmp), int(value));
    }

    void check_float(const char *text, float value)
    {
        float tmp;
        printf("check_float %s\n", text);
        UTEST_ASSERT_MSG(
            sfz::parse_float(&tmp, text) == STATUS_OK,
            "Failed parse integer value text='%s'",
            text);
        UTEST_ASSERT_MSG(
            float_equals_adaptive(tmp, value),
            "Invalid integer value text='%s', value=%f, expected=%f",
            text, tmp, value);
    }

    void check_note(const char *text, ssize_t value)
    {
        ssize_t tmp;
        printf("check_note %s\n", text);
        UTEST_ASSERT_MSG(
            sfz::parse_note(&tmp, text) == STATUS_OK,
            "Failed parse note value text='%s'",
            text);
        UTEST_ASSERT_MSG(
            tmp == value,
            "Invalid note value text='%s', value=%d, expected=%d",
            text, int(tmp), int(value));
    }

    UTEST_MAIN
    {
        // Check boolean parsing
        check_bool("t", true);
        check_bool("f", false);
        check_bool("on", true);
        check_bool("off", false);
        check_bool("yes", true);
        check_bool("no", false);
        check_bool("true", true);
        check_bool("false", false);

        // Check integer parsing
        check_int("0", 0);
        check_int("123", 123);
        check_int("-12345", -12345);

        // Check floating-point parsing
        check_float("0", 0.0f);
        check_float("0.0", 0.0f);
        check_float("440.0", 440.0f);
        check_float("-48000.0", -48000.0f);
        check_float("12.34", 12.34f);

        // Check note parsing
        check_note("35", 35);
        check_note("b1", 35);
        check_note("B1", 35);
        check_note("B#1", 36);
        check_note("B##1", 37);
        check_note("Bb1", 34);
        check_note("Bbb1", 33);
        check_note("A-1", 9);
    }

UTEST_END




