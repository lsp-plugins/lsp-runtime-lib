/*
 * library.cpp
 *
 *  Created on: 5 мая 2020 г.
 *      Author: sadko
 */

#include <lsp-plug.in/test-fw/utest.h>
#include <lsp-plug.in/ipc/Library.h>

UTEST_BEGIN("runtime.ipc", library)

    UTEST_MAIN
    {
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

UTEST_END


