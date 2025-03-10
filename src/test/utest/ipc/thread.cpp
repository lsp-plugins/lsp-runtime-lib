/*
 * Copyright (C) 2025 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2025 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-runtime-lib
 * Created on: 25 февр. 2019 г.
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
#include <lsp-plug.in/ipc/Thread.h>

using namespace lsp;

UTEST_BEGIN("runtime.ipc", thread)
    class TestThread: public ipc::Thread
    {
        private:
            volatile int *res;
            int exit_code;

        public:
            explicit TestThread() { res = NULL; exit_code = -1; }
            virtual ~TestThread() {}

            void bind(volatile int *res, int exit_code) { this->res = res; this->exit_code = exit_code; }

            virtual status_t run()
            {
                while (*res == 0)
                    Thread::sleep(100);

                Thread::sleep(200);
                *res    = 2;
                Thread::sleep(100000000); // We need to leave sleep() on cancel() event
                *res    = 3;

                return exit_code;
            }
    };

    class TestRunnable: public ipc::IRunnable
    {
        private:
            volatile int *res;
            int exit_code;

        public:
            TestRunnable(volatile int *res, int exit_code) { this->res = res; this->exit_code = exit_code; }

            virtual status_t run()
            {
                while (*res == 0)
                    ipc::Thread::sleep(100);

                ipc::Thread::sleep(200);
                *res    = 2;
                ipc::Thread::sleep(100000000); // We need to leave sleep() on cancel() event
                *res    = 3;

                return exit_code;
            }
    };

    typedef struct binding_t
    {
        volatile int *res;
        int exit_code;
    } binding_t;

    static status_t thread_proc(void *arg)
    {
        binding_t *binding = reinterpret_cast<binding_t *>(arg);

        while (*(binding->res) == 0)
            ipc::Thread::sleep(100);

        ipc::Thread::sleep(200);
        *(binding->res) = 2;
        ipc::Thread::sleep(100000000); // We need to leave sleep() on cancel() event
        *(binding->res) = 3;

        return binding->exit_code;
    }

    UTEST_MAIN
    {
        size_t i;
        volatile int res[6];
        ipc::Thread *pt[6];

        for (i=0; i<6; ++i)
            res[i] = 0;

        //---------------------------------------------------------------------
        // Create threads
        printf("Creating threads...\n");

        // Create custom threads
        TestThread t[4];
        for (i=0; i<4; ++i)
        {
            pt[i] = &t[i];
            t[i].bind(&res[i], int(i+1));
        }

        // Create thread as wrapper of runnable
        TestRunnable r(&res[i], int(i+1));
        ipc::Thread rt(&r);
        pt[i++]     = &rt;

        // Create thread as a wrapper around thread_proc
        binding_t binding;
        binding.res         = &res[i];
        binding.exit_code   = int(i + 1);
        ipc::Thread bt(thread_proc, &binding);
        pt[i++]     = &bt;

        //---------------------------------------------------------------------
        // Launch threads
        printf("Launching threads...\n");
        for (i=0; i<6; ++i)
        {
            UTEST_ASSERT(pt[i]->start() == STATUS_OK);
        }

        //---------------------------------------------------------------------
        // Main checking cycle
        printf("Sleeping...\n");
        ipc::Thread::sleep(500);

        printf("Waking threads...\n");
        for (size_t i=0; i<6; ++i)
        {
            UTEST_ASSERT(res[i] == 0);
            res[i] = 1;
        }

        printf("Syncing...\n");
        while (true)
        {
            size_t n = 0;
            for (size_t i=0; i<6; ++i)
            {
                UTEST_ASSERT(res[i] < 2);
                if (res[i] == 1)
                    ++n;
            }
            if (n == 6)
                break;
        }

        //---------------------------------------------------------------------
        // Cancel threads
        printf("Cancelling threads...\n");
        for (i=0; i<6; ++i)
        {
            UTEST_ASSERT(!pt[i]->cancelled());
            UTEST_ASSERT(pt[i]->cancel() == STATUS_OK);
        }

        printf("Waiting threads for termination...\n");
        for (i=0; i<6; ++i)
            pt[i]->join();

        printf("Analyzing exit status...\n");
        for (i=0; i<6; ++i)
        {
            UTEST_ASSERT(pt[i]->cancelled());
            UTEST_ASSERT(size_t(pt[i]->get_result()) == (i+1));
        }
    }
UTEST_END;


