/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-runtime-lib
 * Created on: 21 сент. 2020 г.
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
#include <lsp-plug.in/ipc/Process.h>
#include <lsp-plug.in/ipc/Thread.h>
#include <lsp-plug.in/runtime/system.h>

using namespace lsp;

namespace {
    class StaticTest
    {
        private:
            status_t    nStatus;

        public:
            StaticTest()    { nStatus = STATUS_OK;          }
            ~StaticTest()   { nStatus = STATUS_REMOVED;     }

        public:
            inline status_t status() const { return nStatus; }
    };

    static StaticTest static_test;

    #ifdef PLATFORM_POSIX
        static int atexit_calls = 0;

        void handle_atexit(void)
        {
            ++atexit_calls;
            printf("atexit called for pid=%d\n", int(getpid()));
        }
    #endif /* PLATFORM_POSIX */
};

UTEST_BEGIN("runtime.ipc", process2)

    UTEST_MAIN
    {
        printf("Testing launch of non-existing process\n");

        // Test static data for being not destructed
        UTEST_ASSERT(static_test.status() == STATUS_OK);

        #ifdef PLATFORM_POSIX
            printf("Parent pid=%d\n", int(getpid()));
            atexit(handle_atexit);
        #endif /* PLATFORM_POSIX */

        ipc::Process p;
        p.set_command("some-long-unexisting-command-which-will-fail");
        p.add_arg("1");
        p.add_arg("2");
        p.add_arg("3");

        printf("Launching process...\n");
        UTEST_ASSERT(p.launch() == STATUS_OK);
        printf("Launched process\n");

        UTEST_ASSERT(p.wait() == STATUS_OK);
        printf("Process has exited\n");

        int code = 0;
        UTEST_ASSERT(p.exit_code(&code) == STATUS_OK);

        printf("Exit code = %d\n", code);

        // Test static data for being not destructed and atexit() has not been called
        UTEST_ASSERT(static_test.status() == STATUS_OK);

        #ifdef PLATFORM_POSIX
            UTEST_ASSERT(atexit_calls == 0);
        #endif /* PLATFORM_POSIX */
    }
UTEST_END;

