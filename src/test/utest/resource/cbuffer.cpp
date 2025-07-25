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

UTEST_BEGIN("runtime.resource", cbuffer)

    void test_buffer()
    {
        resource::cbuffer_t buf;
        size_t offset = 0, length = 0;

        printf("Testing compression buffer");

        UTEST_ASSERT(buf.init(0x20) == STATUS_OK);

        // Check that we can not find value in empty buffer
        // buffer state: ???? ???? ???? ???? ???? ???? ???? ????
        //               ^
        UTEST_ASSERT(buf.size() == 0);
        length = buf.lookup(&offset, "a", 1);
        UTEST_ASSERT(length == 0);
        length = buf.lookup(&offset, "b", 1);
        UTEST_ASSERT(length == 0);

        // Append single byte
        // buffer state: a??? ???? ???? ???? ???? ???? ???? ????
        //                ^
        buf.append('a');
        UTEST_ASSERT(buf.size() == 1);
        length = buf.lookup(&offset, "a", 1);
        UTEST_ASSERT(length == 1);
        UTEST_ASSERT(offset == 0);
        length = buf.lookup(&offset, "b", 1);
        UTEST_ASSERT(length == 0);

        // Append small sequence of bytes
        // buffer state: abcd efgh ???? ???? ???? ???? ???? ????
        //                         ^
        buf.append("bcdefgh", 7);
        UTEST_ASSERT(buf.size() == 8);
        length = buf.lookup(&offset, "a", 1);
        UTEST_ASSERT(length == 1);
        UTEST_ASSERT(offset == 7);
        length = buf.lookup(&offset, "b", 1);
        UTEST_ASSERT(length == 1);
        UTEST_ASSERT(offset == 6);
        length = buf.lookup(&offset, "ab", 2);
        UTEST_ASSERT(length == 2);
        UTEST_ASSERT(offset == 7);

        // Append yet another small sequence of bytes
        // buffer state: abcd efgh abc0 1234 ???? ???? ???? ????
        //                                   ^
        buf.append("abc01234", 8);
        UTEST_ASSERT(buf.size() == 16);
        length = buf.lookup(&offset, "a", 1);
        UTEST_ASSERT(length == 1);
        UTEST_ASSERT(offset == 7);
        length = buf.lookup(&offset, "b", 1);
        UTEST_ASSERT(length == 1);
        UTEST_ASSERT(offset == 6);
        length = buf.lookup(&offset, "d", 1);
        UTEST_ASSERT(length == 1);
        UTEST_ASSERT(offset == 12);
        length = buf.lookup(&offset, "abc", 3);
        UTEST_ASSERT(length == 3);
        UTEST_ASSERT(offset == 7);
        length = buf.lookup(&offset, "abcd", 4);
        UTEST_ASSERT(length == 4);
        UTEST_ASSERT(offset == 15);

        // Append more data to the buffer
        // buffer state: abcd efgh abc0 1234 0123 4567 abcd efg?
        //                                                     ^
        buf.append("01234567abcdefg", 15);
        UTEST_ASSERT(buf.size() == 31);
        length = buf.lookup(&offset, "a", 1);
        UTEST_ASSERT(length == 1);
        UTEST_ASSERT(offset == 6);
        length = buf.lookup(&offset, "b", 1);
        UTEST_ASSERT(length == 1);
        UTEST_ASSERT(offset == 5);
        length = buf.lookup(&offset, "abc", 3);
        UTEST_ASSERT(length == 3);
        UTEST_ASSERT(offset == 6);
        length = buf.lookup(&offset, "abcd", 4);
        UTEST_ASSERT(length == 4);
        UTEST_ASSERT(offset == 6);
        length = buf.lookup(&offset, "abcdefghabc", 11);
        UTEST_ASSERT(length == 11);
        UTEST_ASSERT(offset == 30);
        length = buf.lookup(&offset, "abcdefghabcdefgh", 16);
        UTEST_ASSERT(length == 11);
        UTEST_ASSERT(offset == 30);

        // Fill the buffer with overlap
        // buffer state: 0123 4567 abc0 1234 0123 4567 abcd efgh
        //                         ^
        buf.append("h01234567", 9);
        UTEST_ASSERT(buf.size() == 32);
        length = buf.lookup(&offset, "a", 1);
        UTEST_ASSERT(length == 1);
        UTEST_ASSERT(offset == 15);
        length = buf.lookup(&offset, "b", 1);
        UTEST_ASSERT(length == 1);
        UTEST_ASSERT(offset == 14);
        length = buf.lookup(&offset, "abc", 3);
        UTEST_ASSERT(length == 3);
        UTEST_ASSERT(offset == 15);
        length = buf.lookup(&offset, "abcd", 4);
        UTEST_ASSERT(length == 4);
        UTEST_ASSERT(offset == 15);
        length = buf.lookup(&offset, "defgK", 5);
        UTEST_ASSERT(length == 4);
        UTEST_ASSERT(offset == 12);
        length = buf.lookup(&offset, "abcdefghabc", 11);
        UTEST_ASSERT(length == 8);
        UTEST_ASSERT(offset == 15);
        length = buf.lookup(&offset, "abcdefghabcdefgh", 16);
        UTEST_ASSERT(length == 8);
        UTEST_ASSERT(offset == 15);
        length = buf.lookup(&offset, "234567", 6);
        UTEST_ASSERT(length == 6);
        UTEST_ASSERT(offset == 5);
        length = buf.lookup(&offset, "234567abc", 9);
        UTEST_ASSERT(length == 9);
        UTEST_ASSERT(offset == 21);
        length = buf.lookup(&offset, "234567abcd", 10);
        UTEST_ASSERT(length == 10);
        UTEST_ASSERT(offset == 21);

        // Fully overwrite the buffer
        // buffer state: ABCD EFGH IJKL MNOP QRST UVWX YZ01 2345
        //               ^
        buf.append("blablablaABCDEFGHIJKLMNOPQRSTUVWXYZ012345", 41);
        UTEST_ASSERT(buf.size() == 32);
        length = buf.lookup(&offset, "ABCD", 4);
        UTEST_ASSERT(length == 4);
        UTEST_ASSERT(offset == 31);
        length = buf.lookup(&offset, "EFGHIJKL", 8);
        UTEST_ASSERT(length == 8);
        UTEST_ASSERT(offset == 27);
        length = buf.lookup(&offset, "MNOPQRSTUVWX1234", 16);
        UTEST_ASSERT(length == 12);
        UTEST_ASSERT(offset == 19);
        length = buf.lookup(&offset, "YZ012345ABCD", 12);
        UTEST_ASSERT(length == 8);
        UTEST_ASSERT(offset == 7);
    }

    UTEST_MAIN
    {
        test_buffer();
    }

UTEST_END



