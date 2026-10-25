/*
 * Copyright (C) 2026 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2026 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-runtime-lib
 * Created on: 25 окт. 2026 г.
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(PLATFORM_LINUX)
    #include <pthread.h>
#endif /* PLATFORM_LINUX */

using namespace lsp;

namespace
{
    // Size of the virtual address space of the process in kilobytes, -1 if not available
    static ssize_t vm_size_kb()
    {
        ssize_t result = -1;
    #if defined(PLATFORM_LINUX)
        char line[256];
        FILE *fd = fopen("/proc/self/status", "r");
        if (fd == NULL)
            return result;
        while (fgets(line, sizeof(line), fd) != NULL)
        {
            if (strncmp(line, "VmSize:", 7) == 0)
            {
                result = atol(&line[7]);
                break;
            }
        }
        fclose(fd);
    #endif /* PLATFORM_LINUX */
        return result;
    }
} /* namespace */

UTEST_BEGIN("runtime.ipc", thread_join)

    UTEST_TIMELIMIT(30)

    class ShortThread: public ipc::Thread
    {
        private:
            bool                    bMeasure;       // Get the size of the thread's stack

        public:
            ssize_t                 nStackKb;       // Size of the thread's stack in kilobytes, -1 if not known

        public:
            explicit ShortThread(bool measure)
            {
                bMeasure    = measure;
                nStackKb    = -1;
            }

            virtual status_t run()
            {
                // pthread_getattr_np() allocates memory, which can make the C library create
                // a new malloc arena (64 MB of address space on 64-bit systems), so only the
                // warm-up threads call it, before VmSize is measured
                if (!bMeasure)
                    return STATUS_OK;

            #if defined(PLATFORM_LINUX)
                pthread_attr_t attr;
                size_t size = 0;
                if (pthread_getattr_np(pthread_self(), &attr) == 0)
                {
                    if (pthread_attr_getstacksize(&attr, &size) == 0)
                        nStackKb    = size / 1024;
                    pthread_attr_destroy(&attr);
                }
            #endif /* PLATFORM_LINUX */
                return STATUS_OK;
            }
    };

    void wait_finished(ShortThread *t)
    {
        while (!t->finished())
            ipc::Thread::sleep(1);
    }

    // Starts the threads, waits until all of them have finished and only then joins them.
    // Returns the size of the stack of the last thread in kilobytes if measured, -1 otherwise.
    ssize_t join_after_finish(size_t count, size_t batch, bool measure)
    {
        ssize_t stack_kb = -1;
        ShortThread **vt = static_cast<ShortThread **>(malloc(sizeof(ShortThread *) * batch));
        UTEST_ASSERT(vt != NULL);

        for (size_t i=0; i<count; i += batch)
        {
            for (size_t j=0; j<batch; ++j)
            {
                vt[j] = new ShortThread(measure);
                UTEST_ASSERT(vt[j] != NULL);
                UTEST_ASSERT(vt[j]->start() == STATUS_OK);
            }
            for (size_t j=0; j<batch; ++j)
                wait_finished(vt[j]);
            for (size_t j=0; j<batch; ++j)
            {
                UTEST_ASSERT(vt[j]->state() == ipc::TS_FINISHED);
                UTEST_ASSERT(vt[j]->join() == STATUS_OK);
                UTEST_ASSERT(vt[j]->join() == STATUS_OK); // A repeated join() is harmless
                UTEST_ASSERT(vt[j]->result() == STATUS_OK);
                stack_kb = vt[j]->nStackKb;
                delete vt[j];
            }
        }

        free(vt);
        return stack_kb;
    }

    void check_join_after_finish(const char *label, size_t count, size_t batch, ssize_t stack_kb)
    {
        const ssize_t before    = vm_size_kb();
        join_after_finish(count, batch, false);
        const ssize_t after     = vm_size_kb();

        if ((before < 0) || (after < 0) || (stack_kb <= 0))
        {
            printf("%s: %d threads joined, address space not measured\n", label, int(count));
            return;
        }

        // A finished thread that was not really joined keeps its whole stack mapped,
        // so the leak is at least count * stack_kb. A joined thread returns its stack
        // to the C library, which keeps at most a small cache of them.
        const ssize_t growth    = after - before;
        const ssize_t limit     = (ssize_t(count) * stack_kb) / 4;
        printf("%s: %d threads joined, stack %d kB, VmSize %+d kB (limit %d kB)\n",
            label, int(count), int(stack_kb), int(growth), int(limit));
        UTEST_ASSERT_MSG(growth < limit,
            "%s: VmSize grew by %d kB after %d threads were joined (limit %d kB)",
            label, int(growth), int(count), int(limit));
    }

    UTEST_MAIN
    {
        // Warm up the C library's cache of thread stacks and get the size of a stack
        const ssize_t stack_kb  = join_after_finish(16, 16, true);

        // join() of a thread that has already finished must release its resources
        check_join_after_finish("one at a time", 64, 1, stack_kb);
        check_join_after_finish("in batches", 64, 16, stack_kb);
    }

UTEST_END;
