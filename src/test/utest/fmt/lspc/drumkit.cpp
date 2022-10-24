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
                // Read file and create audio chunk
                printf("Writing file data chunk for file '%s'...\n", abs_child.as_native());
                UTEST_ASSERT(lspc::write_audio(&chunk_id, lspc, &abs_child) == STATUS_OK);
                printf("Written as chunk id=%d\n", int(chunk_id));

                // Write the path entry that references the created audio chunk
                printf("Writing path entry chunk '%s' referencing chunk id=%d for file '%s'...\n",
                    child.as_native(), int(chunk_id), abs_child.as_native());
                UTEST_ASSERT(lspc::write_path(&chunk_id, lspc, &child, 0, chunk_id) == STATUS_OK);
                printf("Written as chunk id=%d\n", int(chunk_id));
            }
            else if (attr.type == io::fattr_t::FT_DIRECTORY)
            {
                if (io::Path::is_dots(&item))
                    continue;

                printf("Writing path entry chunk for directory '%s'...\n", abs_child.as_native());
                UTEST_ASSERT(lspc::write_path(&chunk_id, lspc, &item, lspc::PATH_DIR, 0) == STATUS_OK);
                printf("Written as chunk id=%d\n", int(chunk_id));

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

        printf("Creating drumkit file to '%s'...\n", drumkit->as_native());
        UTEST_ASSERT(lspc.create(drumkit) == STATUS_OK);

        printf("Writing audio files...\n");
        write_dir_to_drumkit(&lspc, path, &relative);

        printf("Writing configuration file '%s'...\n", config->as_native());
        UTEST_ASSERT(lspc::write_config(&chunk_id, &lspc, config) == STATUS_OK);
        printf("Written as chunk id=%d\n", int(chunk_id));

        UTEST_ASSERT(lspc.close() == STATUS_OK);
    }

    void enumerate_chunks(lspc::File *lspc, uint32_t chunk_magic, ssize_t required_count)
    {
        lspc::chunk_id_t *list = NULL;
        ssize_t count = lspc->enumerate_chunks(chunk_magic, &list);
        UTEST_ASSERT(count == required_count);
        UTEST_ASSERT(list != NULL);

        for (ssize_t i=0; i<count; ++i)
            printf("  found chunk id=%d\n", int(list[i]));
        free(list);
    }

    void enumerate_all_chunks(lspc::File *lspc, ssize_t required_count)
    {
        lspc::chunk_info_t *list = NULL;
        ssize_t count = lspc->enumerate_chunks(&list);
        UTEST_ASSERT(count == required_count);
        UTEST_ASSERT(list != NULL);

        for (ssize_t i=0; i<count; ++i)
            printf("  found chunk id=%2d magic='%c%c%c%c' position=%lld size=%lld\n",
                int(list[i].chunk_id),
                char((list[i].magic >> 24) & 0xff),
                char((list[i].magic >> 16) & 0xff),
                char((list[i].magic >> 8) & 0xff),
                char((list[i].magic) & 0xff),
                (long long)list[i].position,
                (long long)list[i].size);
        free(list);
    }

    void enumerate_drumkit_chunks(const io::Path *drumkit)
    {
        lspc::File lspc;

        printf("Opening drumkit file...\n");
        UTEST_ASSERT(lspc.open(drumkit) == STATUS_OK);

        printf("Enumerating text configuration chunks...\n");
        enumerate_chunks(&lspc, LSPC_CHUNK_TEXT_CONFIG, 1);
        printf("Enumerating audio chunks...\n");
        enumerate_chunks(&lspc, LSPC_CHUNK_AUDIO, 4);
        printf("Enumerating path chunks...\n");
        enumerate_chunks(&lspc, LSPC_CHUNK_PATH, 5);

        printf("Enumerating all chunks...\n");
        enumerate_all_chunks(&lspc, 10);
    }

    UTEST_MAIN
    {
        io::Path drumkit, src_dir, config;
        UTEST_ASSERT(drumkit.fmt("%s/utest-%s-drumkit.lspc", tempdir(), full_name()));
        UTEST_ASSERT(src_dir.fmt("%s/fmt/lspc/drumkit/data", resources()));
        UTEST_ASSERT(config.fmt("%s/fmt/lspc/drumkit/drumkit.cfg", resources()));

        create_drumkit_file(&drumkit, &src_dir, &config);
        enumerate_drumkit_chunks(&drumkit);
    }

UTEST_END

