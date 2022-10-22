/*
 * Copyright (C) 2022 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2022 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-runtime-lib
 * Created on: 22 окт. 2022 г.
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

#include <lsp-plug.in/fmt/lspc/File.h>
#include <lsp-plug.in/fmt/lspc/util.h>
#include <lsp-plug.in/io/Dir.h>
#include <lsp-plug.in/io/Path.h>
#include <lsp-plug.in/test-fw/utest.h>

UTEST_BEGIN("runtime.fmt.lspc", drumkit)

    UTEST_TIMELIMIT(300)

    void write_dir_to_drumkit(lspc::File *lspc, const io::Path *base, const io::Path *relative)
    {
        io::Dir dir;
        io::fattr_t attr;
        io::Path dir_path, child, abs_child;
        lspc::chunk_id_t chunk_id;
        LSPString item;
        status_t res;

        UTEST_ASSERT(dir_path.set(base, relative) == STATUS_OK);
        UTEST_ASSERT(dir.open(&dir_path) == STATUS_OK);
        while ((res = dir.reads(&item, &attr)) == STATUS_OK)
        {
            UTEST_ASSERT(child.set(relative, &item) == STATUS_OK);
            UTEST_ASSERT(abs_child.set(base, &child) == STATUS_OK);

            if (attr.type == io::fattr_t::FT_REGULAR)
            {
                // TODO: read file and create chunk
                printf("Writing path entry chunk for file '%s'...\n", abs_child.as_native());
                UTEST_ASSERT(lspc::write_path_entry(&chunk_id, lspc, &item, 0, 0) == STATUS_OK);
                printf("Written as chunk id=%d...\n", int(chunk_id));
            }
            else if (attr.type == io::fattr_t::FT_DIRECTORY)
            {
                if (io::Path::is_dots(&item))
                    continue;

                printf("Writing path entry chunk for directory '%s'...\n", abs_child.as_native());
                UTEST_ASSERT(lspc::write_path_entry(&chunk_id, lspc, &item, lspc::PATH_DIR, 0) == STATUS_OK);
                printf("Written as chunk id=%d...\n", int(chunk_id));

                // Perform recursive call
                write_dir_to_drumkit(lspc, base, &child);
            }
        }
        UTEST_ASSERT(res == STATUS_EOF);
    }

    void create_drumkit_file(const io::Path *drumkit, const io::Path *path, const io::Path *config)
    {
        lspc::File lspc;
        lspc::chunk_id_t chunk_id;
        io::Path relative;

        printf("Creating drumkit file to '%s'...", drumkit->as_native());
        UTEST_ASSERT(lspc.create(drumkit) == STATUS_OK);

        printf("Writing audio files...\n");
        write_dir_to_drumkit(&lspc, path, &relative);

        printf("Writing configuration file '%s'...\n", config->as_native());
        UTEST_ASSERT(lspc::write_config_entry(&chunk_id, &lspc, config) == STATUS_OK);
        printf("Written as chunk id=%d...\n", int(chunk_id));
    }

    UTEST_MAIN
    {
        io::Path drumkit, src_dir, config;
        UTEST_ASSERT(drumkit.fmt("%s/utest-%s-drumkit.lspc", tempdir(), full_name()));
        UTEST_ASSERT(src_dir.fmt("%s/fmt/lspc/drumkit/data", resources()));
        UTEST_ASSERT(config.fmt("%s/fmt/lspc/drumkit/drumkit.cfg", resources()));

        create_drumkit_file(&drumkit, &src_dir, &config);
    }

UTEST_END

