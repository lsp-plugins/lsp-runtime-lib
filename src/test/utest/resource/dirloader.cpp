/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-runtime-lib
 * Created on: 26 окт. 2020 г.
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
#include <lsp-plug.in/resource/DirLoader.h>
#include <stdlib.h>

using namespace lsp;

UTEST_BEGIN("runtime.resource", dirloader)

    bool match_resource(const resource::resource_t *item, const resource::resource_t *list, size_t count)
    {
        for (size_t i=0; i<count; ++i)
        {
            if (item->type != list[i].type)
                continue;
            if (strcmp(item->name, list[i].name) == 0)
                return true;
        }
        return false;
    }

    void test_dir(resource::ILoader *loader, const char *path, const resource::resource_t *list, size_t count)
    {
        resource::resource_t *items = NULL;

        printf("Testing directory listing of \"%s\"...\n", path);

        ssize_t nitems = loader->enumerate(path, &items);
        UTEST_ASSERT(nitems == ssize_t(count));
        UTEST_ASSERT(items != NULL);

        for (ssize_t i=0; i<nitems; ++i)
        {
            UTEST_ASSERT_MSG(match_resource(&items[i], list, count), "Unexpected resource: %s", items[i].name);
        }

        free(items);
    }

    void test_file(resource::ILoader *loader, const char *path, const char *data)
    {
        printf("Testing read of resource \"%s\"...\n", path);
        LSPString line;

        io::IInSequence *is = loader->read_sequence(path, "UTF-8");
        UTEST_ASSERT(is != NULL);
        UTEST_ASSERT(is->read_line(&line, true) == STATUS_OK);
        UTEST_ASSERT(is->read_line(&line, true) == STATUS_EOF)
        UTEST_ASSERT(is->close() == STATUS_OK);
        delete is;

        UTEST_ASSERT(line.equals_ascii(data));
    }

    void test_unexisting(resource::ILoader *loader, const char *path)
    {
        printf("Testing read of unexisting resource \"%s\"...\n", path);
        io::IInSequence *is = loader->read_sequence(path, "UTF-8");
        UTEST_ASSERT(is == NULL);
        UTEST_ASSERT(loader->last_error() != STATUS_OK);
    }

    UTEST_MAIN
    {
        static const resource::resource_t list1[] =
        {
            { resource::RES_DIR,  "dir" },
            { resource::RES_FILE, "file.txt" },
            { resource::RES_FILE, "test.txt" }
        };

        static const resource::resource_t list2[] =
        {
            { resource::RES_FILE, "item.txt" }
        };

        resource::DirLoader loader;
        loader.set_enforce(true);
        loader.set_path(resources());

        printf("Resource directory: %s\n", resources());

        test_dir(&loader, "resource", list1, sizeof(list1)/sizeof(resource::resource_t));
        test_dir(&loader, "../resource", list1, sizeof(list1)/sizeof(resource::resource_t));
        test_dir(&loader, "./resource", list1, sizeof(list1)/sizeof(resource::resource_t));
        test_dir(&loader, "resource/dir", list2, sizeof(list2)/sizeof(resource::resource_t));

        test_file(&loader, "resource/file.txt", "file.txt");
        test_file(&loader, "resource/test.txt", "test.txt");
        test_file(&loader, "resource/dir/item.txt", "item.txt");
        test_unexisting(&loader, "resource/dir/unexisting.txt");
    }

UTEST_END


