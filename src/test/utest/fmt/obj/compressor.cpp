/*
 * Copyright (C) 2025 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2025 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-runtime-lib
 * Created on: 23 июл. 2025 г.
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
#include <lsp-plug.in/fmt/obj/Compressor.h>
#include <lsp-plug.in/fmt/obj/PushParser.h>
#include <stdarg.h>

UTEST_BEGIN("runtime.fmt.obj", compressor)

    void test_compress_obj_file(const char *fname, const char *postfix)
    {
        io::Path src, dst;
        UTEST_ASSERT(src.fmt("%s/%s", resources(), fname) > 0);
        UTEST_ASSERT(dst.fmt("%s/utest-%s-%s.cobj", tempdir(), full_name(), postfix) > 0);

        printf("Converting %s -> %s\n", src.as_native(), dst.as_native());

        obj::Compressor c;
        obj::PushParser p;

        UTEST_ASSERT(c.set_buffer_size(5, 5) == STATUS_OK);
        UTEST_ASSERT(c.open(&dst, io::File::FM_WRITE_NEW) == STATUS_OK);
        UTEST_ASSERT(p.parse_file(&c, &src) == STATUS_OK);
        UTEST_ASSERT(c.close() == STATUS_OK);

        const wssize_t src_size = src.size();
        UTEST_ASSERT(src_size >= 0);
        const wssize_t dst_size = dst.size();
        UTEST_ASSERT(dst_size >= 0);

        printf("Compression ratio: %.2f %%\n", 100.0 * double(src_size) / double(dst_size));
    }

    UTEST_MAIN
    {
        printf("Testing compression of complicated OBJ file...\n");
        test_compress_obj_file("fmt/obj/parking.obj", "parking");
        printf("Testing compression of complicated OBJ file...\n");
        test_compress_obj_file("fmt/obj/coliseum.obj", "coliseum");
        printf("Testing compression of complicated OBJ file...\n");
        test_compress_obj_file("fmt/obj/forest.obj", "forest");
        printf("Testing compression of complicated OBJ file...\n");
        test_compress_obj_file("fmt/obj/cooling-tower.obj", "cooling-tower");
    }

UTEST_END


