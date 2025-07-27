/*
 * Copyright (C) 2025 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2025 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-runtime-lib
 * Created on: 25 июл. 2025 г.
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
#include <lsp-plug.in/resource/buffer.h>

using namespace lsp;

UTEST_BEGIN("runtime.resource", dbuffer)

    UTEST_MAIN
    {
        resource::dbuffer_t buf;
        uint8_t tmp[0x20];

        printf("Testing decompression buffer");

        UTEST_ASSERT(buf.init(0x20) == STATUS_OK);

        // Check that we can not extract data from empty buffer
        // buffer state: ???? ???? ???? ???? ???? ???? ???? ????
        //               ^
        UTEST_ASSERT(buf.size() == 0);
        UTEST_ASSERT(buf.extract(tmp, 0, 1) == STATUS_UNDERFLOW);

        // Append single byte
        // buffer state: a??? ???? ???? ???? ???? ???? ???? ????
        //                ^
        buf.append('a');
        UTEST_ASSERT(buf.size() == 1);
        UTEST_ASSERT(buf.extract(tmp, 0, 1) == STATUS_OK);
        UTEST_ASSERT(tmp[0] == 'a');
        UTEST_ASSERT(buf.extract(tmp, 0, 2) == STATUS_UNDERFLOW);
        UTEST_ASSERT(buf.extract(tmp, 1, 1) == STATUS_UNDERFLOW);

        // Append small sequence of bytes
        // buffer state: abcd efgh ???? ???? ???? ???? ???? ????
        //                         ^
        buf.append("bcdefgh", 7);
        UTEST_ASSERT(buf.size() == 8);
        UTEST_ASSERT(buf.extract(tmp, 0, 1) == STATUS_OK);
        UTEST_ASSERT(tmp[0] == 'h');
        UTEST_ASSERT(buf.extract(tmp, 6, 1) == STATUS_OK);
        UTEST_ASSERT(tmp[0] == 'b');
        UTEST_ASSERT(buf.extract(tmp, 7, 8) == STATUS_OK);
        UTEST_ASSERT(memcmp(tmp, "abcdefgh", 8) == 0);
        UTEST_ASSERT(buf.extract(tmp, 3, 4) == STATUS_OK);
        UTEST_ASSERT(memcmp(tmp, "efgh", 4) == 0);
        UTEST_ASSERT(buf.extract(tmp, 3, 8) == STATUS_UNDERFLOW);
        UTEST_ASSERT(buf.extract(tmp, 7, 9) == STATUS_UNDERFLOW);

        // Append yet another small sequence of bytes
        // buffer state: abcd efgh abc0 1234 ???? ???? ???? ????
        //                                   ^
        buf.append("abc01234", 8);
        UTEST_ASSERT(buf.size() == 16);
        UTEST_ASSERT(buf.extract(tmp, 7, 1) == STATUS_OK);
        UTEST_ASSERT(tmp[0] == 'a');
        UTEST_ASSERT(buf.extract(tmp, 0, 1) == STATUS_OK);
        UTEST_ASSERT(tmp[0] == '4');
        UTEST_ASSERT(buf.extract(tmp, 7, 8) == STATUS_OK);
        UTEST_ASSERT(memcmp(tmp, "abc01234", 8) == 0);
        UTEST_ASSERT(buf.extract(tmp, 15, 16) == STATUS_OK);
        UTEST_ASSERT(memcmp(tmp, "abcdefghabc01234", 16) == 0);

        // Append more data to the buffer
        // buffer state: abcd efgh abc0 1234 0123 4567 abcd efg?
        //                                                     ^
        buf.append("01234567abcdefg", 15);
        UTEST_ASSERT(buf.size() == 31);
        UTEST_ASSERT(buf.extract(tmp, 0, 1) == STATUS_OK);
        UTEST_ASSERT(tmp[0] == 'g');
        UTEST_ASSERT(buf.extract(tmp, 7, 8) == STATUS_OK);
        UTEST_ASSERT(memcmp(tmp, "7abcdefg", 8) == 0);
        UTEST_ASSERT(buf.extract(tmp, 15, 16) == STATUS_OK);
        UTEST_ASSERT(memcmp(tmp, "401234567abcdefg", 16) == 0);

        // Fill the buffer with overlap
        // buffer state: 0123 4567 abc0 1234 0123 4567 abcd efgh
        //                         ^
        buf.append("h01234567", 9);
        UTEST_ASSERT(buf.size() == 32);
        UTEST_ASSERT(buf.extract(tmp, 31, 32) == STATUS_OK);
        UTEST_ASSERT(memcmp(tmp, "abc0123401234567abcdefgh01234567", 32) == 0);
        UTEST_ASSERT(buf.extract(tmp, 7, 8) == STATUS_OK);
        UTEST_ASSERT(memcmp(tmp, "01234567", 8) == 0);

        // Fully overwrite the buffer
        // buffer state: ABCD EFGH IJKL MNOP QRST UVWX YZ01 2345
        //               ^
        buf.append("blablablaABCDEFGHIJKLMNOPQRSTUVWXYZ012345", 41);
        UTEST_ASSERT(buf.size() == 32);
        UTEST_ASSERT(buf.extract(tmp, 31, 32) == STATUS_OK);
        UTEST_ASSERT(memcmp(tmp, "ABCDEFGHIJKLMNOPQRSTUVWXYZ012345", 32) == 0);
    }

UTEST_END


