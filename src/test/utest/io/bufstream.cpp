/*
 * Copyright (C) 2026 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2026 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-runtime-lib
 * Created on: 17 янв. 2026 г.
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

#include <lsp-plug.in/io/InBufStream.h>
#include <lsp-plug.in/io/InMemoryStream.h>
#include <lsp-plug.in/test-fw/utest.h>

UTEST_BEGIN("runtime.io", inbufstream)

    void test_unbuffered_reads(io::InMemoryStream & ims)
    {
        printf("Testing unbuffered reads\n");

        const uint8_t * const src = ims.data();
        uint8_t buf[16];
        io::InBufStream is(8);
        is.set_buffering(false);
        is.set_seek_buffering(false);

        UTEST_FOREACH(size, 1, 2, 3, 5, 7, 8, 11, 13, 16) {
            printf("  block_size=%d ...\n", int(size));

            UTEST_ASSERT(ims.seek(0) == 0);
            UTEST_ASSERT(is.wrap(&ims) == STATUS_OK);

            size_t offset = 0;
            while (true)
            {
                ssize_t n_read = is.read(buf, size);
                if (n_read < 0)
                {
                    UTEST_ASSERT(n_read == -STATUS_EOF);
                    break;
                }

                UTEST_ASSERT_MSG(
                    memcmp(buf, &src[offset], n_read) == 0,
                    "Failed block size=%d, offset=%d",
                    int(size), int(offset));
                offset += n_read;

                UTEST_ASSERT_MSG(
                    is.position() == wssize_t(offset),
                    "Invalid position returned %d, expected %d",
                    int(is.position()), int(offset));
            }

            UTEST_ASSERT_MSG(
                is.position() == wssize_t(offset),
                "Invalid position returned %d, expected %d",
                int(is.position()), int(offset));
            UTEST_ASSERT(offset == ims.size());
            UTEST_ASSERT(is.close() == STATUS_OK);
        }
    }

    void test_unbuffered_byte_reads(io::InMemoryStream & ims)
    {
        printf("Testing unbuffered byte reads\n");

        const uint8_t * const src = ims.data();
        io::InBufStream is(8);
        is.set_buffering(false);
        is.set_seek_buffering(false);

        UTEST_ASSERT(ims.seek(0) == 0);
        UTEST_ASSERT(is.wrap(&ims) == STATUS_OK);

        size_t offset = 0;
        while (true)
        {
            ssize_t n_read = is.read_byte();
            if (n_read < 0)
            {
                UTEST_ASSERT(n_read == -STATUS_EOF);
                break;
            }

            UTEST_ASSERT_MSG(
                src[offset] == n_read,
                "Failed read byte at offset=%d",
                int(offset));
            ++offset;

            UTEST_ASSERT_MSG(
                is.position() == wssize_t(offset),
                "Invalid position returned %d, expected %d",
                int(is.position()), int(offset));
        }

        UTEST_ASSERT_MSG(
            is.position() == wssize_t(offset),
            "Invalid position returned %d, expected %d",
            int(is.position()), int(offset));
        UTEST_ASSERT(offset == ims.size());
        UTEST_ASSERT(is.close() == STATUS_OK);
    }

    void test_buffered_reads(io::InMemoryStream & ims)
    {
        printf("Testing buffered reads\n");

        const uint8_t * const src = ims.data();
        uint8_t buf[16];
        io::InBufStream is(8);
        is.set_buffering(false);
        is.set_seek_buffering(false);

        UTEST_FOREACH(size, 1, 2, 3, 5, 7, 8, 11, 13, 16) {
            printf("  block_size=%d ...\n", int(size));

            UTEST_ASSERT(ims.seek(0) == 0);
            UTEST_ASSERT(is.wrap(&ims) == STATUS_OK);

            size_t offset = 0;
            while (true)
            {
                ssize_t n_read = is.read(buf, size);
                if (n_read < 0)
                {
                    UTEST_ASSERT(n_read == -STATUS_EOF);
                    break;
                }

                UTEST_ASSERT_MSG(
                    memcmp(buf, &src[offset], n_read) == 0,
                    "Failed block size=%d, offset=%d",
                    int(size), int(offset));
                offset += n_read;

                UTEST_ASSERT_MSG(
                    is.position() == wssize_t(offset),
                    "Invalid position returned %d, expected %d",
                    int(is.position()), int(offset));
            }

            UTEST_ASSERT_MSG(
                is.position() == wssize_t(offset),
                "Invalid position returned %d, expected %d",
                int(is.position()), int(offset));
            UTEST_ASSERT(offset == ims.size());
            UTEST_ASSERT(is.close() == STATUS_OK);
        }
    }

    void test_buffered_byte_reads(io::InMemoryStream & ims)
    {
        printf("Testing buffered byte reads\n");

        const uint8_t * const src = ims.data();
        io::InBufStream is(8);
        is.set_buffering(true);
        is.set_seek_buffering(false);

        UTEST_ASSERT(ims.seek(0) == 0);
        UTEST_ASSERT(is.wrap(&ims) == STATUS_OK);

        size_t offset = 0;
        while (true)
        {
            ssize_t n_read = is.read_byte();
            if (n_read < 0)
            {
                UTEST_ASSERT(n_read == -STATUS_EOF);
                break;
            }

            UTEST_ASSERT_MSG(
                src[offset] == n_read,
                "Failed read byte at offset=%d",
                int(offset));
            ++offset;

            UTEST_ASSERT_MSG(
                is.position() == wssize_t(offset),
                "Invalid position returned %d, expected %d",
                int(is.position()), int(offset));
        }

        UTEST_ASSERT_MSG(
            is.position() == wssize_t(offset),
            "Invalid position returned %d, expected %d",
            int(is.position()), int(offset));
        UTEST_ASSERT(offset == ims.size());
        UTEST_ASSERT(is.close() == STATUS_OK);
    }

    void test_buffered_rereads_non_buffered_seeks(io::InMemoryStream & ims)
    {
        printf("Testing buffered re-reads with non-buffered seeks\n");

        const uint8_t * const src = ims.data();
        uint8_t buf[24];
        io::InBufStream is(8);
        is.set_buffering(true);
        is.set_seek_buffering(false);

        UTEST_FOREACH(size, 1, 2, 3, 5, 7, 8, 11, 13, 16, 23) {
            printf("  block_size=%d ...\n", int(size));

            UTEST_ASSERT(ims.seek(0) == 0);
            UTEST_ASSERT(is.wrap(&ims) == STATUS_OK);

            size_t offset       = 0;
            const size_t half   = lsp_max(size / 2, size_t(1));
            while (true)
            {
                // Perform first read
                ssize_t n_read = is.read(buf, size);
                if (n_read < 0)
                {
                    UTEST_ASSERT(n_read == -STATUS_EOF);
                    break;
                }

                UTEST_ASSERT_MSG(
                    memcmp(buf, &src[offset], n_read) == 0,
                    "Failed block size=%d, offset=%d",
                    int(size), int(offset));

                UTEST_ASSERT_MSG(
                    is.position() == wssize_t(offset + n_read),
                    "Invalid position returned %d, expected %d",
                    int(is.position()), int(offset + n_read));

                // Perform second read
                UTEST_ASSERT(is.seek(offset) == wssize_t(offset));
                UTEST_ASSERT_MSG(
                    is.position() == wssize_t(offset),
                    "Invalid position returned %d, expected %d",
                    int(is.position()), int(offset));

                n_read = is.read(buf, half);
                if (n_read < 0)
                {
                    UTEST_ASSERT(n_read == -STATUS_EOF);
                    break;
                }

                UTEST_ASSERT_MSG(
                    memcmp(buf, &src[offset], n_read) == 0,
                    "Failed block size=%d, offset=%d",
                    int(size), int(offset));
                offset += n_read;

                UTEST_ASSERT_MSG(
                    is.position() == wssize_t(offset),
                    "Invalid position returned %d, expected %d",
                    int(is.position()), int(offset));
            }

            UTEST_ASSERT_MSG(
                is.position() == wssize_t(offset),
                "Invalid position returned %d, expected %d",
                int(is.position()), int(offset));
            UTEST_ASSERT(offset == ims.size());
            UTEST_ASSERT(is.close() == STATUS_OK);
        }
    }

    void test_buffered_byte_rereads_non_buffered_seeks(io::InMemoryStream & ims)
    {
        printf("Testing buffered byte re-reads with non-buffered seeks\n");

        const uint8_t * const src = ims.data();
        uint8_t buf[24];
        io::InBufStream is(8);
        is.set_buffering(true);
        is.set_seek_buffering(false);

        UTEST_FOREACH(size, 1, 2, 3, 5, 7, 8, 11, 13, 16, 23) {
            printf("  block_size=%d ...\n", int(size));

            UTEST_ASSERT(ims.seek(0) == 0);
            UTEST_ASSERT(is.wrap(&ims) == STATUS_OK);

            size_t offset       = 0;
            while (true)
            {
                // Perform first read
                ssize_t n_read = is.read(buf, size);
                if (n_read < 0)
                {
                    UTEST_ASSERT(n_read == -STATUS_EOF);
                    break;
                }

                UTEST_ASSERT_MSG(
                    memcmp(buf, &src[offset], n_read) == 0,
                    "Failed block size=%d, offset=%d",
                    int(size), int(offset));

                UTEST_ASSERT_MSG(
                    is.position() == wssize_t(offset + n_read),
                    "Invalid position returned %d, expected %d",
                    int(is.position()), int(offset + n_read));

                // Perform second read
                UTEST_ASSERT(is.seek(offset) == wssize_t(offset));
                UTEST_ASSERT_MSG(
                    is.position() == wssize_t(offset),
                    "Invalid position returned %d, expected %d",
                    int(is.position()), int(offset));

                n_read = is.read_byte();
                if (n_read < 0)
                {
                    UTEST_ASSERT(n_read == -STATUS_EOF);
                    break;
                }

                UTEST_ASSERT_MSG(
                    n_read == src[offset],
                    "Failed read_byte at offset=%d",
                    int(offset));
                ++offset;

                UTEST_ASSERT_MSG(
                    is.position() == wssize_t(offset),
                    "Invalid position returned %d, expected %d",
                    int(is.position()), int(offset));
            }

            UTEST_ASSERT_MSG(
                is.position() == wssize_t(offset),
                "Invalid position returned %d, expected %d",
                int(is.position()), int(offset));
            UTEST_ASSERT(offset == ims.size());
            UTEST_ASSERT(is.close() == STATUS_OK);
        }
    }

    void test_buffered_rereads_with_buffered_seeks(io::InMemoryStream & ims)
    {
        printf("Testing buffered re-reads with buffered seeks\n");

        const uint8_t * const src = ims.data();
        uint8_t buf[24];
        io::InBufStream is(8);
        is.set_buffering(true);
        is.set_seek_buffering(true);

        UTEST_FOREACH(size, 1, 2, 3, 5, 7, 8, 11, 13, 16, 23) {
            printf("  block_size=%d ...\n", int(size));

            UTEST_ASSERT(ims.seek(0) == 0);
            UTEST_ASSERT(is.wrap(&ims) == STATUS_OK);

            size_t offset       = 0;
            const size_t half   = lsp_max(size / 2, size_t(1));
            while (true)
            {
                // Perform first read
                ssize_t n_read = is.read(buf, size);
//                printf("    n_read=%d\n", int(n_read));
                if (n_read < 0)
                {
                    UTEST_ASSERT(n_read == -STATUS_EOF);
                    break;
                }

                UTEST_ASSERT_MSG(
                    memcmp(buf, &src[offset], n_read) == 0,
                    "Failed block size=%d, offset=%d",
                    int(size), int(offset));

                UTEST_ASSERT_MSG(
                    is.position() == wssize_t(offset + n_read),
                    "Invalid position returned %d, expected %d",
                    int(is.position()), int(offset + n_read));

                // Perform second read
                UTEST_ASSERT(is.seek(offset) == wssize_t(offset));
                UTEST_ASSERT_MSG(
                    is.position() == wssize_t(offset),
                    "Invalid position returned %d, expected %d",
                    int(is.position()), int(offset));

                n_read = is.read(buf, half);
//                printf("    n_read=%d\n", int(n_read));
                if (n_read < 0)
                {
                    UTEST_ASSERT(n_read == -STATUS_EOF);
                    break;
                }

                UTEST_ASSERT_MSG(
                    memcmp(buf, &src[offset], n_read) == 0,
                    "Failed block size=%d, offset=%d",
                    int(size), int(offset));
                offset += n_read;

                UTEST_ASSERT_MSG(
                    is.position() == wssize_t(offset),
                    "Invalid position returned %d, expected %d",
                    int(is.position()), int(offset));
            }

            UTEST_ASSERT_MSG(
                is.position() == wssize_t(offset),
                "Invalid position returned %d, expected %d",
                int(is.position()), int(offset));
            UTEST_ASSERT(offset == ims.size());
            UTEST_ASSERT(is.close() == STATUS_OK);
        }
    }

    void test_buffered_byte_rereads_with_buffered_seeks(io::InMemoryStream & ims)
    {
        printf("Testing buffered byte re-reads with buffered seeks\n");

        const uint8_t * const src = ims.data();
        uint8_t buf[24];
        io::InBufStream is(8);
        is.set_buffering(true);
        is.set_seek_buffering(true);

        UTEST_FOREACH(size, 1, 2, 3, 5, 7, 8, 11, 13, 16, 23) {
            printf("  block_size=%d ...\n", int(size));

            UTEST_ASSERT(ims.seek(0) == 0);
            UTEST_ASSERT(is.wrap(&ims) == STATUS_OK);

            size_t offset       = 0;
            while (true)
            {
                // Perform first read
                ssize_t n_read = is.read(buf, size);
//                printf("    n_read=%d\n", int(n_read));
                if (n_read < 0)
                {
                    UTEST_ASSERT(n_read == -STATUS_EOF);
                    break;
                }

                UTEST_ASSERT_MSG(
                    memcmp(buf, &src[offset], n_read) == 0,
                    "Failed block size=%d, offset=%d",
                    int(size), int(offset));

                UTEST_ASSERT_MSG(
                    is.position() == wssize_t(offset + n_read),
                    "Invalid position returned %d, expected %d",
                    int(is.position()), int(offset + n_read));

                // Perform second read
                UTEST_ASSERT(is.seek(offset) == wssize_t(offset));
                UTEST_ASSERT_MSG(
                    is.position() == wssize_t(offset),
                    "Invalid position returned %d, expected %d",
                    int(is.position()), int(offset));

                n_read = is.read_byte();
//                printf("    n_read=%d\n", int(n_read));
                if (n_read < 0)
                {
                    UTEST_ASSERT(n_read == -STATUS_EOF);
                    break;
                }

                UTEST_ASSERT_MSG(
                    n_read == src[offset],
                    "Failed read_byte at offset=%d",
                    int(offset));
                ++offset;

                UTEST_ASSERT_MSG(
                    is.position() == wssize_t(offset),
                    "Invalid position returned %d, expected %d",
                    int(is.position()), int(offset));
            }

            UTEST_ASSERT_MSG(
                is.position() == wssize_t(offset),
                "Invalid position returned %d, expected %d",
                int(is.position()), int(offset));
            UTEST_ASSERT(offset == ims.size());
            UTEST_ASSERT(is.close() == STATUS_OK);
        }
    }

    void test_unbuffered_skip_unbuffered_rereads(io::InMemoryStream & ims)
    {
        printf("Testing unbuffered skip + unbuffered re-reads\n");

        const uint8_t * const src = ims.data();
        uint8_t buf[24];
        io::InBufStream is(8);
        is.set_buffering(false);
        is.set_seek_buffering(false);

        UTEST_FOREACH(size, 1, 2, 3, 5, 7, 8, 11, 13, 16, 23) {
            printf("  block_size=%d ...\n", int(size));

            UTEST_ASSERT(ims.seek(0) == 0);
            UTEST_ASSERT(is.wrap(&ims) == STATUS_OK);

            size_t offset       = 0;
            const size_t half   = lsp_max(size / 2, size_t(1));
            while (true)
            {
                // Perform skip
                ssize_t n_read = is.skip(size);
//                printf("    n_skip=%d\n", int(n_read));
                UTEST_ASSERT(n_read >= 0);
                if (n_read == 0)
                    break;

                offset += n_read;
                UTEST_ASSERT_MSG(
                    is.position() == wssize_t(offset),
                    "Invalid position returned %d, expected %d",
                    int(is.position()), int(offset));

                n_read  = is.read(buf, half);
//                printf("    n_read=%d\n", int(n_read));
                if (n_read < 0)
                {
                    UTEST_ASSERT(n_read == -STATUS_EOF);
                    break;
                }
                UTEST_ASSERT_MSG(
                    memcmp(buf, &src[offset], n_read) == 0,
                    "Failed block size=%d, offset=%d",
                    int(size), int(offset));
                offset += n_read;
                UTEST_ASSERT_MSG(
                    is.position() == wssize_t(offset),
                    "Invalid position returned %d, expected %d",
                    int(is.position()), int(offset));

                // Perform backward seek
                offset -= size;
                UTEST_ASSERT(is.seek(offset) == wssize_t(offset));
                UTEST_ASSERT_MSG(
                    is.position() == wssize_t(offset),
                    "Invalid position returned %d, expected %d",
                    int(is.position()), int(offset));

                n_read = is.read(buf, half);
//                printf("    n_read=%d\n", int(n_read));
                if (n_read < 0)
                {
                    UTEST_ASSERT(n_read == -STATUS_EOF);
                    break;
                }

                UTEST_ASSERT_MSG(
                    memcmp(buf, &src[offset], n_read) == 0,
                    "Failed block size=%d, offset=%d",
                    int(size), int(offset));
                offset += n_read;

                UTEST_ASSERT_MSG(
                    is.position() == wssize_t(offset),
                    "Invalid position returned %d, expected %d",
                    int(is.position()), int(offset));
            }

            UTEST_ASSERT_MSG(
                is.position() == wssize_t(offset),
                "Invalid position returned %d, expected %d",
                int(is.position()), int(offset));
            UTEST_ASSERT(offset == ims.size());
            UTEST_ASSERT(is.close() == STATUS_OK);
        }
    }

    void test_unbuffered_skip_buffered_rereads(io::InMemoryStream & ims)
    {
        printf("Testing unbuffered skip + buffered re-reads\n");

        const uint8_t * const src = ims.data();
        uint8_t buf[24];
        io::InBufStream is(8);
        is.set_buffering(true);
        is.set_seek_buffering(false);

        UTEST_FOREACH(size, 1, 2, 3, 5, 7, 8, 11, 13, 16, 23) {
            printf("  block_size=%d ...\n", int(size));

            UTEST_ASSERT(ims.seek(0) == 0);
            UTEST_ASSERT(is.wrap(&ims) == STATUS_OK);

            size_t offset       = 0;
            const size_t half   = lsp_max(size / 2, size_t(1));
            while (true)
            {
                // Perform skip
                ssize_t n_read = is.skip(size);
//                printf("    n_skip=%d\n", int(n_read));
                UTEST_ASSERT(n_read >= 0);
                if (n_read == 0)
                    break;

                offset += n_read;
                UTEST_ASSERT_MSG(
                    is.position() == wssize_t(offset),
                    "Invalid position returned %d, expected %d",
                    int(is.position()), int(offset));

                n_read  = is.read(buf, half);
//                printf("    n_read=%d\n", int(n_read));
                if (n_read < 0)
                {
                    UTEST_ASSERT(n_read == -STATUS_EOF);
                    break;
                }
                UTEST_ASSERT_MSG(
                    memcmp(buf, &src[offset], n_read) == 0,
                    "Failed block size=%d, offset=%d",
                    int(size), int(offset));
                offset += n_read;
                UTEST_ASSERT_MSG(
                    is.position() == wssize_t(offset),
                    "Invalid position returned %d, expected %d",
                    int(is.position()), int(offset));

                // Perform backward seek
                offset -= size;
                UTEST_ASSERT(is.seek(offset) == wssize_t(offset));
                UTEST_ASSERT_MSG(
                    is.position() == wssize_t(offset),
                    "Invalid position returned %d, expected %d",
                    int(is.position()), int(offset));

                n_read = is.read(buf, half);
//                printf("    n_read=%d\n", int(n_read));
                if (n_read < 0)
                {
                    UTEST_ASSERT(n_read == -STATUS_EOF);
                    break;
                }

                UTEST_ASSERT_MSG(
                    memcmp(buf, &src[offset], n_read) == 0,
                    "Failed block size=%d, offset=%d",
                    int(size), int(offset));
                offset += n_read;

                UTEST_ASSERT_MSG(
                    is.position() == wssize_t(offset),
                    "Invalid position returned %d, expected %d",
                    int(is.position()), int(offset));
            }

            UTEST_ASSERT_MSG(
                is.position() == wssize_t(offset),
                "Invalid position returned %d, expected %d",
                int(is.position()), int(offset));
            UTEST_ASSERT(offset == ims.size());
            UTEST_ASSERT(is.close() == STATUS_OK);
        }
    }

    void test_buffered_skip_buffered_rereads(io::InMemoryStream & ims)
    {
        printf("Testing buffered skip + buffered re-reads\n");

        const uint8_t * const src = ims.data();
        uint8_t buf[24];
        io::InBufStream is(8);
        is.set_buffering(true);
        is.set_seek_buffering(true);

        UTEST_FOREACH(size, /*1,*/ 2, 3, 5, 7, 8, 11, 13, 16, 23) {
            printf("  block_size=%d ...\n", int(size));

            UTEST_ASSERT(ims.seek(0) == 0);
            UTEST_ASSERT(is.wrap(&ims) == STATUS_OK);

            size_t offset       = 0;
            const size_t half   = lsp_max(size / 2, size_t(1));
            while (true)
            {
                // Perform skip
                ssize_t n_read = is.skip(size);
//                printf("    n_skip=%d\n", int(n_read));
                UTEST_ASSERT(n_read >= 0);
                if (n_read == 0)
                    break;

                offset += n_read;
                UTEST_ASSERT_MSG(
                    is.position() == wssize_t(offset),
                    "Invalid position returned %d, expected %d",
                    int(is.position()), int(offset));

                n_read  = is.read(buf, half);
//                printf("    n_read=%d\n", int(n_read));
                if (n_read < 0)
                {
                    UTEST_ASSERT(n_read == -STATUS_EOF);
                    break;
                }
                UTEST_ASSERT_MSG(
                    memcmp(buf, &src[offset], n_read) == 0,
                    "Failed block size=%d, offset=%d",
                    int(size), int(offset));
                offset += n_read;
                UTEST_ASSERT_MSG(
                    is.position() == wssize_t(offset),
                    "Invalid position returned %d, expected %d",
                    int(is.position()), int(offset));

                // Perform backward seek
                offset -= size;
                UTEST_ASSERT(is.seek(offset) == wssize_t(offset));
                UTEST_ASSERT_MSG(
                    is.position() == wssize_t(offset),
                    "Invalid position returned %d, expected %d",
                    int(is.position()), int(offset));

                n_read = is.read(buf, half);
//                printf("    n_read=%d\n", int(n_read));
                if (n_read < 0)
                {
                    UTEST_ASSERT(n_read == -STATUS_EOF);
                    break;
                }

                UTEST_ASSERT_MSG(
                    memcmp(buf, &src[offset], n_read) == 0,
                    "Failed block size=%d, offset=%d",
                    int(size), int(offset));
                offset += n_read;

                UTEST_ASSERT_MSG(
                    is.position() == wssize_t(offset),
                    "Invalid position returned %d, expected %d",
                    int(is.position()), int(offset));
            }

            UTEST_ASSERT_MSG(
                is.position() == wssize_t(offset),
                "Invalid position returned %d, expected %d",
                int(is.position()), int(offset));
            UTEST_ASSERT(offset == ims.size());
            UTEST_ASSERT(is.close() == STATUS_OK);
        }
    }

    UTEST_MAIN
    {
        uint8_t *data       = new uint8_t[0x80];
        UTEST_ASSERT(data != NULL);
        for (size_t i=0; i<0x80; ++i)
            data[i]             = uint8_t(i + 1);

        io::InMemoryStream ims;
        ims.wrap(data, 0x80, MEMDROP_ARR_DELETE);

        test_unbuffered_reads(ims);
        test_unbuffered_byte_reads(ims);
        test_buffered_reads(ims);
        test_buffered_byte_reads(ims);
        test_buffered_rereads_non_buffered_seeks(ims);
        test_buffered_byte_rereads_non_buffered_seeks(ims);
        test_buffered_rereads_with_buffered_seeks(ims);
        test_buffered_byte_rereads_with_buffered_seeks(ims);
        test_unbuffered_skip_unbuffered_rereads(ims);
        test_unbuffered_skip_buffered_rereads(ims);
        test_buffered_skip_buffered_rereads(ims);
    }
UTEST_END


