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
#include <lsp-plug.in/common/alloc.h>
#include <lsp-plug.in/io/Path.h>
#include <lsp-plug.in/io/Dir.h>
#include <lsp-plug.in/io/InFileStream.h>
#include <lsp-plug.in/io/OutFileStream.h>
#include <lsp-plug.in/resource/Compressor.h>
#include <lsp-plug.in/resource/Decompressor.h>
#include <lsp-plug.in/resource/BuiltinLoader.h>

using namespace lsp;

#define BUFFER_SIZE     0x100000

UTEST_BEGIN("runtime.resource", compressor)

    UTEST_TIMELIMIT(30)

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

    void scan_resources(resource::BuiltinLoader *load, const io::Path *path, const io::Path *temp, const io::Path *rel)
    {
        resource::resource_t *rlist = NULL;
        io::Path file, child, out;
        io::OutMemoryStream oms1, oms2;
        io::InFileStream ifs;
        io::IInStream *irs;
        io::OutFileStream ofs;

        ssize_t items   = load->enumerate(rel, &rlist);
        UTEST_ASSERT(items >= 0);
        UTEST_ASSERT(rlist != NULL);

        for (ssize_t i=0; i<items; ++i)
        {
            resource::resource_t *item = &rlist[i];

            UTEST_ASSERT(child.set(rel, item->name) == STATUS_OK);
            printf("  found entry: %s\n", child.as_native());

            if (item->type == resource::RES_DIR)
            {
                scan_resources(load, path, temp, &child);
                continue;
            }

            // Decompress the item
            oms1.clear();
            UTEST_ASSERT((irs = load->read_stream(&child)) != NULL);
            wssize_t sz1 = irs->sink(&oms1);
            UTEST_ASSERT(sz1 >= 0);
            UTEST_ASSERT(irs->close() == STATUS_OK);
            UTEST_ASSERT(oms1.size() == size_t(sz1));
            printf("  decompressed entry size: %ld bytes\n", long(sz1));

            // Save the decompressed entry
            UTEST_ASSERT(out.set(temp, rel) == STATUS_OK);
            UTEST_ASSERT(out.append_child(item->name) == STATUS_OK);
            printf("    saving decompressed entry as: %s\n", out.as_native());
            UTEST_ASSERT(out.mkparent(true) == STATUS_OK);
            UTEST_ASSERT(ofs.open(&out, io::File::FM_WRITE_NEW) == STATUS_OK);
            wssize_t osz = ofs.write(oms1.data(), oms1.size());
            UTEST_ASSERT(osz == sz1);
            UTEST_ASSERT(ofs.close() == STATUS_OK);

            // Read the original file
            oms2.clear();
            UTEST_ASSERT(file.set(path, &child) == STATUS_OK);
            UTEST_ASSERT(ifs.open(&file) == STATUS_OK);
            wssize_t sz2 = ifs.sink(&oms2);
            UTEST_ASSERT(ifs.close() == STATUS_OK);
            UTEST_ASSERT(sz2 >= 0);
            UTEST_ASSERT(oms2.size() == size_t(sz2));
            printf("    original entry size: %ld bytes\n", long(sz2));

            // Compare the data
            UTEST_ASSERT(sz1 == sz2);
            UTEST_ASSERT(memcmp(oms1.data(), oms2.data(), sz1) == 0);
            oms1.drop();
            oms2.drop();
        }

        // Free items
        free(rlist);
    }

    void test_compress_data(const io::Path *path, resource::Compressor *c)
    {
        wsize_t data_size = 0;
        io::Path tmp;
        io::OutFileStream ofs;

        // Scan and compress directory
        UTEST_ASSERT(c->init(BUFFER_SIZE) == STATUS_OK);
        printf("Scanning source directory...\n");
        scan_directory(&data_size, path, path, c);
        c->flush(); // Flush compressor if ther is some data

        size_t buf_sz = c->commands_size();
        double ratio = double(data_size) / double(buf_sz);

        printf("Command size: %d, data size: %d, ratio: %.2f\n",
            int(c->commands_size()),
            int(data_size),
            ratio
        );

        UTEST_ASSERT(tmp.fmt("%s/%s.commands", tempdir(), full_name()) > 0);
        printf("Dumping commands to: %s\n", tmp.as_native());
        UTEST_ASSERT(ofs.open(&tmp, io::File::FM_WRITE_NEW) == STATUS_OK);
        ofs.write(c->commands(), c->commands_size());
        UTEST_ASSERT(ofs.close() == STATUS_OK);
    }

    void test_decompress_data(const io::Path *path, resource::Compressor *c)
    {
        resource::BuiltinLoader load;
        io::Path rel;
        io::Path tmp;

        UTEST_ASSERT(tmp.fmt("%s/utest-%s", tempdir(), full_name()) > 0);
        UTEST_ASSERT(load.init(c->commands(), c->commands_size(), c->entries(), c->num_entires(), BUFFER_SIZE) == STATUS_OK);
        printf("Scanning resource registry...\n");
        scan_resources(&load, path, &tmp, &rel);
    }

    UTEST_MAIN
    {
        io::Path path;
        resource::Compressor c;

        UTEST_ASSERT(path.fmt("%s/compressor", resources()) > 0);
        printf("Resource directory: %s\n", path.as_native());

        // Compress data
        test_compress_data(&path, &c);

        // Decompress data
        test_decompress_data(&path, &c);

        UTEST_ASSERT(c.close() == STATUS_OK);
    }

UTEST_END


