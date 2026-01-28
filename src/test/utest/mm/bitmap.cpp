/*
 * Copyright (C) 2026 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2026 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-runtime-lib
 * Created on: 29 янв. 2026 г.
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
#include <lsp-plug.in/common/endian.h>
#include <lsp-plug.in/mm/Bitmap.h>

static const char *bitmap =
    "10000000"
    "10022200"
    "10200330"
    "10200303"
    "11122303"
    "00000330"
    "00000300"
    "00222300";

static const uint32_t colors[] =
{
    0xff000000,
    0xff0000ff,
    0xff00ff00,
    0xffff0000
};

UTEST_BEGIN("runtime.mm", bitmap)

    void test_load_bitmap(const char *file, bool prgba)
    {
        io::Path path;

        UTEST_ASSERT(path.fmt("%s/mm/bitmap/%s", resources(), file));
        printf("Reading %s bitmap from file %s\n", (prgba) ? "PRGBA" : "RGBA", path.as_native());

        mm::Bitmap bmp;
        UTEST_ASSERT(bmp.load(&path, (prgba) ? mm::PIXFMT_PRGBA8888 : mm::PIXFMT_RGBA8888) == STATUS_OK);
        UTEST_ASSERT(bmp.width() == 8);
        UTEST_ASSERT(bmp.height() == 8);

        for (size_t y=0; y<8; ++y)
        {
            const uint32_t * const row = reinterpret_cast<const uint32_t *>(bmp.row(y));
            UTEST_ASSERT(row != NULL);

            for (size_t x=0; x<8; ++x)
            {
                const uint32_t rc = colors[bitmap[y*8 + x] - '0'];
                const uint32_t sc = LE_TO_CPU(row[x]);
                UTEST_ASSERT_MSG(rc == sc, "Failed color check at point (%d, %d)", int(x), int(y));
            }
        }
    }

    UTEST_MAIN
    {
        test_load_bitmap("test.xpm", false);
        test_load_bitmap("test.xpm", true);
    }
UTEST_END;





