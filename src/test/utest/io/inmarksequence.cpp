/*
 * Copyright (C) 2021 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2021 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-runtime-lib
 * Created on: 12 февр. 2021 г.
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
#include <lsp-plug.in/test-fw/ByteBuffer.h>
#include <lsp-plug.in/io/InMarkSequence.h>
#include <lsp-plug.in/io/InStringSequence.h>

UTEST_BEGIN("runtime.io", inmarksequence)

    bool check_string(const LSPString *s, size_t first, size_t len)
    {
        if (s->length() != len)
            return false;
        for (size_t i=0; i<len; ++i)
            if (s->char_at(i) != (first + i + 1))
                return false;
        return true;
    }

    bool check_buffer(const lsp_wchar_t *b, size_t first, size_t count)
    {
        for (size_t i=0; i<count; ++i)
            if (b[i] != (first + i))
            {
                printf("b[%d] = %d != %d\n", i, b[i], int(first + i));
                return false;
            }
        return true;
    }

    void test_full_read(const LSPString *s)
    {
        printf("Testing full read of %d characters\n", int(s->length()));

        io::InStringSequence in(s);
        io::InMarkSequence ims;
        LSPString out;
        ByteBuffer buf(s->length() * sizeof(lsp_wchar_t));
        lsp_wchar_t *data = buf.data<lsp_wchar_t>();

        // Test for mark() and wrap
        UTEST_ASSERT(ims.mark(-1) == STATUS_CLOSED);
        UTEST_ASSERT(ims.reset() == STATUS_CLOSED);
        UTEST_ASSERT(ims.wrap(&in, true) == STATUS_OK);
        UTEST_ASSERT(ims.mark(-1) == STATUS_OK);
        UTEST_ASSERT(ims.reset() == STATUS_NOT_FOUND);

        // Perform first read
        ssize_t nread = ims.read(data, s->length() * 2);
        UTEST_ASSERT(nread == ssize_t(s->length()));
        UTEST_ASSERT(!buf.corrupted());
        UTEST_ASSERT(check_buffer(data, 1, s->length()));

        // Perform second read - EOF should be returned
        nread = ims.read(data, s->length() * 2);
        UTEST_ASSERT(nread == -STATUS_EOF);

        // reset() should return NOT_FOUND error
        UTEST_ASSERT(ims.reset() == STATUS_NOT_FOUND);

        // Close the stream
        UTEST_ASSERT(ims.close() == STATUS_OK);
    }

    void test_partial_read(const LSPString *s, size_t blk_size)
    {
        printf("Testing partial read for block size %d\n", int(blk_size));

        io::InStringSequence in(s);
        io::InMarkSequence ims;
        LSPString out;
        ByteBuffer buf(s->length() * sizeof(lsp_wchar_t));
        lsp_wchar_t *data = buf.data<lsp_wchar_t>();

        // Wrap
        UTEST_ASSERT(ims.wrap(&in, true) == STATUS_OK);
        ssize_t nread = 0, total = 0;

        // Perform read with specific block size
        while ((nread = ims.read(data, blk_size)) >= 0)
        {
            data       += nread;
            total      += nread;
        }

        UTEST_ASSERT(nread == -STATUS_EOF);
        UTEST_ASSERT(total == ssize_t(s->length()));

        // Check buffer
        data = buf.data<lsp_wchar_t>();
        UTEST_ASSERT(check_buffer(data, 1, s->length()));

        // Close the stream
        UTEST_ASSERT(ims.close() == STATUS_OK);
    }

    void test_character_read(const LSPString *s)
    {
        printf("Testing character read\n");

        io::InStringSequence in(s);
        io::InMarkSequence ims;
        LSPString out;
        ByteBuffer buf(s->length() * sizeof(lsp_wchar_t));
        lsp_wchar_t *data = buf.data<lsp_wchar_t>();

        // Wrap
        UTEST_ASSERT(ims.wrap(&in, true) == STATUS_OK);
        ssize_t total = 0;
        lsp_swchar_t ch;

        // Perform read with specific block size
        while ((ch = ims.read()) >= 0)
            data[total++] = ch;

        UTEST_ASSERT(ch == -STATUS_EOF);
        UTEST_ASSERT(total == ssize_t(s->length()));

        // Check buffer
        data = buf.data<lsp_wchar_t>();
        UTEST_ASSERT(check_buffer(data, 1, s->length()));

        // Close the stream
        UTEST_ASSERT(ims.close() == STATUS_OK);
    }

    void test_full_with_mark(const LSPString *s)
    {
        printf("Testing full read of %d characters with mark\n", int(s->length()));

        io::InStringSequence in(s);
        io::InMarkSequence ims;
        LSPString out;
        ByteBuffer buf(s->length() * sizeof(lsp_wchar_t) * 2);
        lsp_wchar_t *data = buf.data<lsp_wchar_t>();

        // Test for mark() and wrap
        UTEST_ASSERT(ims.wrap(&in, true) == STATUS_OK);
        UTEST_ASSERT(ims.mark(s->length()) == STATUS_OK);

        // Perform first read
        ssize_t nread = ims.read(data, s->length() * 2);
        UTEST_ASSERT(nread == ssize_t(s->length()));
        UTEST_ASSERT(!buf.corrupted());
        UTEST_ASSERT(check_buffer(data, 1, s->length()));

        // Perform reset and second read
        UTEST_ASSERT(ims.reset() == STATUS_OK);
        nread = ims.read(&data[s->length()], s->length() * 2);
        UTEST_ASSERT(nread == ssize_t(s->length()));
        UTEST_ASSERT(!buf.corrupted());
        UTEST_ASSERT(check_buffer(&data[s->length()], 1, s->length()));

        // Perform third read - EOF should be returned
        nread = ims.read(&data[s->length() * 2], s->length() * 2);
        UTEST_ASSERT(!buf.corrupted());
        UTEST_ASSERT(nread == -STATUS_EOF);

        // Close the stream
        UTEST_ASSERT(ims.close() == STATUS_OK);
    }

    void test_character_with_mark(const LSPString *s)
    {
        printf("Testing full read of %d characters with mark\n", int(s->length()));

        io::InStringSequence in(s);
        io::InMarkSequence ims;
        LSPString out;
        ByteBuffer buf(s->length() * sizeof(lsp_wchar_t) * 2);
        lsp_wchar_t *data = buf.data<lsp_wchar_t>();

        // Test for mark() and wrap
        UTEST_ASSERT(ims.wrap(&in, true) == STATUS_OK);
        UTEST_ASSERT(ims.mark(s->length()) == STATUS_OK);

        // Perform first read
        ssize_t total = 0;
        lsp_swchar_t ch;
        while ((ch = ims.read()) >= 0)
            data[total++] = ch;

        UTEST_ASSERT(ch == -STATUS_EOF);
        UTEST_ASSERT(total == ssize_t(s->length()));
        UTEST_ASSERT(!buf.corrupted());
        UTEST_ASSERT(check_buffer(data, 1, s->length()));

        // Perform reset and second read
        UTEST_ASSERT(ims.reset() == STATUS_OK);
        while ((ch = ims.read()) >= 0)
            data[total++] = ch;

        UTEST_ASSERT(ch == -STATUS_EOF);
        UTEST_ASSERT(total == ssize_t(s->length() * 2));
        UTEST_ASSERT(!buf.corrupted());
        UTEST_ASSERT(check_buffer(&data[s->length()], 1, s->length()));

        // Close the stream
        UTEST_ASSERT(ims.close() == STATUS_OK);
    }

    void test_partial_with_mark(const LSPString *s, ssize_t start, ssize_t mark, ssize_t blk_size)
    {
        printf("Testing partial read of %d-%d/%d characters with mark\n", int(start), int(mark), int(blk_size));

        io::InStringSequence in(s);
        io::InMarkSequence ims;
        LSPString out;
        ByteBuffer buf(s->length() * sizeof(lsp_wchar_t) * 3);
        lsp_wchar_t *data = buf.data<lsp_wchar_t>();
        lsp_wchar_t *ptr = data;

        // Test for mark() and wrap
        UTEST_ASSERT(ims.wrap(&in, true) == STATUS_OK);
        ssize_t nread = 0, total = 0;

        // Perform read with specific block size
        while (total < start)
        {
            nread       = ims.read(data, lsp_min(blk_size, start - total));
            UTEST_ASSERT(nread >= 0);
            data       += nread;
            total      += nread;
        }
        UTEST_ASSERT(total == start);
        UTEST_ASSERT(check_buffer(ptr, 1, total));

        // Perform mark and read next value
        UTEST_ASSERT(ims.mark(mark) == STATUS_OK);
        ptr += total;
        total = 0;
        while (total < mark)
        {
            nread       = ims.read(data, lsp_min(blk_size, mark - total));
            UTEST_ASSERT(nread >= 0);
            data       += nread;
            total      += nread;
        }
        UTEST_ASSERT(total == mark);
        UTEST_ASSERT(check_buffer(ptr, start + 1, total));

        // Perform reset and read again
        UTEST_ASSERT(ims.reset() == STATUS_OK);
        ptr += total;
        total = 0;
        while (total < mark*2)
        {
            nread       = ims.read(data, lsp_min(blk_size, mark*2 - total));
            UTEST_ASSERT(nread >= 0);
            data       += nread;
            total      += nread;
        }
        UTEST_ASSERT(total == mark*2);
        UTEST_ASSERT(check_buffer(ptr, start + 1, total));

        // Now reset() should fail, read the rest data
        UTEST_ASSERT(ims.reset() == STATUS_NOT_FOUND);
        ptr += total;
        total = 0;
        while ((nread = ims.read(data, blk_size)) >= 0)
        {
            data       += nread;
            total      += nread;
        }

        UTEST_ASSERT(nread == -STATUS_EOF);
        UTEST_ASSERT(total == ssize_t(s->length() - start - mark*2));
        UTEST_ASSERT(check_buffer(ptr, start + mark*2 + 1, total));

        // Close the stream
        UTEST_ASSERT(ims.close() == STATUS_OK);
    }

    void test_char_partial_with_mark(const LSPString *s, ssize_t start, ssize_t mark)
    {
        printf("Testing partial character read of %d-%d characters with mark\n", int(start), int(mark));

        io::InStringSequence in(s);
        io::InMarkSequence ims;
        LSPString out;
        ByteBuffer buf(s->length() * sizeof(lsp_wchar_t) * 3);
        lsp_wchar_t *data = buf.data<lsp_wchar_t>();
        lsp_wchar_t *ptr = data;
        lsp_swchar_t ch;

        // Test for mark() and wrap
        UTEST_ASSERT(ims.wrap(&in, true) == STATUS_OK);
        ssize_t total = 0;

        // Perform initial read
        while (total < start)
        {
            UTEST_ASSERT((ch = ims.read()) >= 0);
            *data++ = ch;
            total++;
        }
        UTEST_ASSERT(total == start);
        UTEST_ASSERT(check_buffer(ptr, 1, total));

        // Perform mark and read next value
        UTEST_ASSERT(ims.mark(mark) == STATUS_OK);
        ptr = data;
        total = 0;
        while (total < mark)
        {
            UTEST_ASSERT((ch = ims.read()) >= 0);
            *data++ = ch;
            total++;
        }
        UTEST_ASSERT(total == mark);
        UTEST_ASSERT(check_buffer(ptr, start + 1, total));

        // Perform reset and read again
        UTEST_ASSERT(ims.reset() == STATUS_OK);
        ptr += total;
        total = 0;
        while (total < mark*2)
        {
            UTEST_ASSERT((ch = ims.read()) >= 0);
            *data++ = ch;
            total++;
        }
        UTEST_ASSERT(total == mark*2);
        UTEST_ASSERT(check_buffer(ptr, start + 1, total));

        // Now reset() should fail, read the rest data
        UTEST_ASSERT(ims.reset() == STATUS_NOT_FOUND);
        ptr += total;
        total = 0;
        while ((ch = ims.read()) >= 0)
        {
            *data++ = ch;
            total++;
        }

        UTEST_ASSERT(ch == -STATUS_EOF);
        UTEST_ASSERT(total == ssize_t(s->length() - start - mark*2));
        UTEST_ASSERT(check_buffer(ptr, start + mark*2 + 1, total));

        // Close the stream
        UTEST_ASSERT(ims.close() == STATUS_OK);
    }

    UTEST_MAIN
    {
        LSPString src;
        for (size_t i=0; i<10000; ++i)
        {
            UTEST_ASSERT(src.append(lsp_wchar_t(i + 1)));
        }
        UTEST_ASSERT(check_string(&src, 0, 10000));

        // Perform tests
        test_full_read(&src);
        test_partial_read(&src, 1);
        test_partial_read(&src, 31);
        test_partial_read(&src, 1023);
        test_partial_read(&src, 1024);
        test_character_read(&src);

        test_full_with_mark(&src);
        test_character_with_mark(&src);

        test_partial_with_mark(&src, 2048, 512,  512);
        test_partial_with_mark(&src, 2048, 1024, 512);
        test_partial_with_mark(&src, 2048, 2048, 512);
        test_partial_with_mark(&src, 2048, 512,  31);
        test_partial_with_mark(&src, 2048, 1024, 31);
        test_partial_with_mark(&src, 2048, 2048, 31);
        test_partial_with_mark(&src, 2048, 512,  1);
        test_partial_with_mark(&src, 2048, 1024, 1);
        test_partial_with_mark(&src, 2048, 2048, 1);

        test_char_partial_with_mark(&src, 2048, 512);
        test_char_partial_with_mark(&src, 2048, 1024);
        test_char_partial_with_mark(&src, 2048, 2048);
        test_char_partial_with_mark(&src, 2048, 512);
        test_char_partial_with_mark(&src, 2048, 1024);
        test_char_partial_with_mark(&src, 2048, 2048);
        test_char_partial_with_mark(&src, 2048, 512);
        test_char_partial_with_mark(&src, 2048, 1024);
        test_char_partial_with_mark(&src, 2048, 2048);
    }
UTEST_END





