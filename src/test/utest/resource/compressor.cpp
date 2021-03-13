/*
 * Copyright (C) 2021 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2021 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-runtime-lib
 * Created on: 3 мар. 2021 г.
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
#include <lsp-plug.in/resource/Compressor.h>
#include <lsp-plug.in/io/Path.h>
#include <lsp-plug.in/io/Dir.h>
#include <lsp-plug.in/io/InFileStream.h>
#include <lsp-plug.in/io/OutFileStream.h>

using namespace lsp;

#define BUFFER_SIZE     0x100000

UTEST_BEGIN("runtime.resource", compressor)

    void scan_directory(wsize_t *data_size, const io::Path *base, const io::Path *path, resource::Compressor *c)
    {
        io::Dir dir;
        status_t res;
        LSPString str;
        io::Path child, relative;
        io::fattr_t fattr;

        UTEST_ASSERT(dir.open(path) == STATUS_OK);
        while ((res = dir.reads(&str, &fattr, false)) == STATUS_OK)
        {
            if (io::Path::is_dots(&str))
                continue;

            UTEST_ASSERT(child.set(path, &str) == STATUS_OK);
            UTEST_ASSERT(relative.set(path, &str) == STATUS_OK);
            UTEST_ASSERT(relative.remove_base(base) == STATUS_OK);

            printf("  found entry: %s\n", relative.as_native());

            if (fattr.type == io::fattr_t::FT_REGULAR)
            {
                io::InFileStream ifs;
                UTEST_ASSERT(ifs.open(&child) == STATUS_OK);
                wssize_t len = c->create_file(&relative, &ifs);
                UTEST_ASSERT(c >= 0);
                UTEST_ASSERT(ifs.close() == STATUS_OK);

                *data_size += len;
            }
            else if (fattr.type == io::fattr_t::FT_DIRECTORY)
            {
                UTEST_ASSERT(c->create_dir(&relative) == STATUS_OK);
                scan_directory(data_size, base, &child, c);
            }
        }
    }

    UTEST_MAIN
    {
        io::Path path;
        resource::Compressor c;
        io::OutFileStream ofs;
        wsize_t data_size = 0;

        UTEST_ASSERT(path.fmt("%s/compressor", resources()) > 0);
        printf("Resource directory: %s\n", path.as_native());

        // Scan and compress directory
        UTEST_ASSERT(c.init(BUFFER_SIZE) == STATUS_OK);

        scan_directory(&data_size, &path, &path, &c);

        size_t buf_sz = c.commands_size();
        double ratio = double(data_size) / double(buf_sz);

        printf("Command size: %d, data size: %d, ratio: %.2f\n",
            int(c.commands_size()),
            int(data_size),
            ratio
        );

        UTEST_ASSERT(path.fmt("%s/%s.commands", tempdir(), full_name()) > 0);
        printf("Dumping commands to: %s\n", path.as_native());
        UTEST_ASSERT(ofs.open(&path, io::File::FM_WRITE_NEW) == STATUS_OK);
        ofs.write(c.commands(), c.commands_size());
        UTEST_ASSERT(ofs.close() == STATUS_OK);

        UTEST_ASSERT(c.close() == STATUS_OK);
    }

UTEST_END


