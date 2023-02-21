/*
 * Copyright (C) 2023 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2023 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-runtime-lib
 * Created on: 22 февр. 2023 г.
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
#include <lsp-plug.in/io/InSharedMemoryStream.h>

UTEST_BEGIN("runtime.io", insharedmemorystream)

    void test_simple_share()
    {
        ByteBuffer src(0x100);
        ByteBuffer dst1(0x100);
        ByteBuffer dst2(0x100);
        src.randomize();
        dst1.randomize();
        dst2.randomize();

        // Do in limited context
        {
            io::InSharedMemoryStream a(src.data<void>(), src.size());
            UTEST_ASSERT(a.references() == 1);

            io::InSharedMemoryStream b = a;
            UTEST_ASSERT(a.references() == 2);
            UTEST_ASSERT(b.references() == 2);

            UTEST_ASSERT(a.avail() == ssize_t(src.size()));
            UTEST_ASSERT(b.avail() == ssize_t(src.size()));

            // Read first (step 1)
            UTEST_ASSERT(a.read(dst1.data<void>(a.position()), 0x20) == 0x20);
            UTEST_ASSERT(a.avail() == ssize_t(src.size() - 0x20));
            UTEST_ASSERT(b.avail() == ssize_t(src.size()));
            UTEST_ASSERT(a.position() == 0x20);
            UTEST_ASSERT(b.position() == 0x00);

            // Read second (step 1)
            UTEST_ASSERT(b.read(dst2.data<void>(b.position()), 0xe0) == 0xe0);
            UTEST_ASSERT(a.avail() == ssize_t(src.size() - 0x20));
            UTEST_ASSERT(b.avail() == ssize_t(src.size() - 0xe0));
            UTEST_ASSERT(a.position() == 0x20);
            UTEST_ASSERT(b.position() == 0xe0);

            // Read first (step 2)
            UTEST_ASSERT(a.read(dst1.data<void>(a.position()), 0x100) == 0xe0);
            UTEST_ASSERT(a.avail() == 0);
            UTEST_ASSERT(b.avail() == ssize_t(src.size() - 0xe0));
            UTEST_ASSERT(a.position() == 0x100);
            UTEST_ASSERT(b.position() == 0xe0);

            // Read second (step 2)
            UTEST_ASSERT(b.read(dst2.data<void>(b.position()), 0x100) == 0x20);
            UTEST_ASSERT(a.avail() == 0);
            UTEST_ASSERT(b.avail() == 0);
            UTEST_ASSERT(a.position() == 0x100);
            UTEST_ASSERT(b.position() == 0x100);

            // Read first (step 3)
            UTEST_ASSERT(a.read(dst1.data<void>(a.position()), 0x100) == -STATUS_EOF);
            UTEST_ASSERT(a.avail() == 0);
            UTEST_ASSERT(b.avail() == 0);
            UTEST_ASSERT(a.position() == 0x100);
            UTEST_ASSERT(b.position() == 0x100);

            // Read second (step 3)
            UTEST_ASSERT(b.read(dst2.data<void>(b.position()), 0x100) == -STATUS_EOF);
            UTEST_ASSERT(a.avail() == 0);
            UTEST_ASSERT(b.avail() == 0);
            UTEST_ASSERT(a.position() == 0x100);
            UTEST_ASSERT(b.position() == 0x100);
        }

        // Check buffers
        UTEST_ASSERT(!src.corrupted());
        UTEST_ASSERT(!dst1.corrupted());
        UTEST_ASSERT(!dst2.corrupted());
        UTEST_ASSERT(src.equals(dst1));
        UTEST_ASSERT(src.equals(dst2));
    }

    UTEST_MAIN
    {
        test_simple_share();
    }
UTEST_END








