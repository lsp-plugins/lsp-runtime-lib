/*
 * Copyright (C) 2025 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2025 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-runtime-lib
 * Created on: 18 мар. 2019 г.
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
#include <lsp-plug.in/io/Dir.h>
#include <lsp-plug.in/io/File.h>

using namespace lsp;

UTEST_BEGIN("runtime.io", dir)

    void testReadDir(const char *dir, bool full)
    {
        io::Dir dh;
        io::Path path;
        LSPString xdir;
        size_t n_read1 = 0, n_read2 = 0;

        UTEST_ASSERT(xdir.set_utf8(dir));

        // Try to read non-empty directory
        UTEST_ASSERT(dh.open(&xdir) == STATUS_OK);
        printf("Reading directory %s...\n", xdir.get_native());
        while (dh.read(&path, full) == STATUS_OK)
        {
            if (path.is_dot() || path.is_dotdot())
                continue;

            printf("  found file: %s\n", path.as_native());
            ++n_read1;
        }
        UTEST_ASSERT(dh.last_error() == STATUS_EOF);
        UTEST_ASSERT(n_read1 >= 7);

        // Try to re-read non-empty directory
        UTEST_ASSERT(dh.rewind() == STATUS_OK);
        printf("Re-reading directory %s...\n", xdir.get_native());
        while (dh.read(&path, full) == STATUS_OK)
        {
            if (path.is_dot() || path.is_dotdot())
                continue;

            printf("  found file: %s\n", path.as_native());
            ++n_read2;
        }
        UTEST_ASSERT(dh.last_error() == STATUS_EOF);
        UTEST_ASSERT(n_read1 == n_read2);

        // Try to re-read non-empty directory
        n_read2 = 0;
        io::fattr_t attr;
        UTEST_ASSERT(dh.rewind() == STATUS_OK);
        printf("Re-reading directory with stat %s...\n", xdir.get_native());
        while (dh.reads(&path, &attr, full) == STATUS_OK)
        {
            if (path.is_dot() || path.is_dotdot())
                continue;
            const char *type = "unknown";

            switch (attr.type)
            {
                case io::fattr_t::FT_BLOCK: type = "block"; break;
                case io::fattr_t::FT_CHARACTER: type = "char"; break;
                case io::fattr_t::FT_DIRECTORY: type = "dir"; break;
                case io::fattr_t::FT_FIFO: type = "fifo"; break;
                case io::fattr_t::FT_SYMLINK: type = "symlink"; break;
                case io::fattr_t::FT_REGULAR: type = "regular"; break;
                case io::fattr_t::FT_SOCKET: type = "socket"; break;
                default: type = "unknown"; break;
            }

            printf("  found file: %s, type=%s, size=%ld\n", path.as_native(), type, long(attr.size));
            ++n_read2;
        }
        UTEST_ASSERT(dh.last_error() == STATUS_EOF);

        UTEST_ASSERT(dh.close() == STATUS_OK);
    }

    void testCreateDir(const char *path, bool cwd)
    {
        io::Path bp, p, xp;
        if (cwd)
        {
            UTEST_ASSERT(p.current() == STATUS_OK);
            printf("Current path is: %s\n", p.as_native());
        }
        else
        {
            UTEST_ASSERT(p.set(tempdir()) == STATUS_OK);
            printf("Temporary path is: %s\n", p.as_native());
        }

        UTEST_ASSERT(bp.set(&p) == STATUS_OK);
        UTEST_ASSERT(p.append_child(full_name()) == STATUS_OK);

        printf("Creating directory: %s\n", p.as_native());
        UTEST_ASSERT(p.mkdir() == STATUS_OK);
        UTEST_ASSERT(p.mkdir() == STATUS_OK);

        UTEST_ASSERT(xp.set(path) == STATUS_OK);
        UTEST_ASSERT(p.append_child(&xp) == STATUS_OK);
        printf("Will try to create subpath: %s\n", p.as_native());
        UTEST_ASSERT(p.mkdir(true) == STATUS_OK);

        printf("Removing: %s\n", p.as_native());
        UTEST_ASSERT(io::File::remove(&p) == STATUS_IS_DIRECTORY);
        UTEST_ASSERT(io::Dir::remove(&p) == STATUS_OK);
        UTEST_ASSERT(io::Dir::remove(&bp) != STATUS_OK);

        // Recursively remove directory until we reach the base path
        UTEST_ASSERT(p.parent() == STATUS_OK);
        do
        {
            printf("Removing: %s\n", p.as_native());
            UTEST_ASSERT(io::Dir::remove(&p) == STATUS_OK);
            UTEST_ASSERT(p.parent() == STATUS_OK);
        } while (!p.equals(&bp));
    }

    UTEST_MAIN
    {
        char path[PATH_MAX];
        char c = FILE_SEPARATOR_C;

        ::snprintf(path, sizeof(path), "%s" "%c" "io" "%c" "iconv",
                resources(), c, c);

        testReadDir(path, false);
        testReadDir(path, true);

        ::snprintf(path, sizeof(path), "some" "%c" "long" "%c" "path", c, c);
        testCreateDir(path, true);

        ::snprintf(path, sizeof(path), "another" "%c" "long" "%c" "path", c, c);
        testCreateDir(path, false);
    }

UTEST_END



