/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-runtime-lib
 * Created on: 5 мая 2020 г.
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
#include <lsp-plug.in/ipc/Library.h>

namespace
{
    static const uint32_t tag = 0;
} /* namespace */

UTEST_BEGIN("runtime.ipc", library)

    void test_library_name()
    {
        printf("Testing validity of library file name\n");

        static const char *lib1 = "library.dll";
        static const char *lib2 = "library.so";
        static const char *lib3 = "path/to/library.dll";
        static const char *lib4 = "path/to/library.so";

        static const char *lib5 = "library" FILE_LIBRARY_EXT_S;
        static const char *lib6 = "path/to/library" FILE_LIBRARY_EXT_S;

    #ifdef PLATFORM_WINDOWS
        UTEST_ASSERT(ipc::Library::valid_library_name(lib1));
        UTEST_ASSERT(!ipc::Library::valid_library_name(lib2));
        UTEST_ASSERT(ipc::Library::valid_library_name(lib3));
        UTEST_ASSERT(!ipc::Library::valid_library_name(lib4));
    #endif

    #ifdef PLATFORM_UNIX_COMPATIBLE
        UTEST_ASSERT(!ipc::Library::valid_library_name(lib1));
        UTEST_ASSERT(ipc::Library::valid_library_name(lib2));
        UTEST_ASSERT(!ipc::Library::valid_library_name(lib3));
        UTEST_ASSERT(ipc::Library::valid_library_name(lib4));
    #endif

        UTEST_ASSERT(ipc::Library::valid_library_name(lib5));
        UTEST_ASSERT(ipc::Library::valid_library_name(lib6));
    }

    void test_module_filename()
    {
        printf("Testing obtaining module file name\n");

        LSPString path;
        UTEST_ASSERT(ipc::Library::get_module_file(&path, &tag) == STATUS_OK);
        printf("Module file name is: %s\n", path.get_native());
    }

    UTEST_MAIN
    {
        test_library_name();
        test_module_filename();
    }

UTEST_END


