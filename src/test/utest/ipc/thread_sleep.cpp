/*
 * Copyright (C) 2026 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2026 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-runtime-lib
 * Created on: 15 окт. 2026 г.
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
#include <lsp-plug.in/common/atomic.h>
#include <lsp-plug.in/ipc/Thread.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(PLATFORM_WINDOWS)
    #include <windows.h>
#else
    #include <time.h>
#endif /* PLATFORM_WINDOWS */

using namespace lsp;

namespace
{
    static const wssize_t NANOS_PER_MILLI   = 1000000;
    static const wssize_t NANOS_PER_SECOND  = 1000000000;

    // Time of the monotonic clock in nanoseconds, does not depend on system time changes
    static wssize_t monotonic_nanos()
    {
    #if defined(PLATFORM_WINDOWS)
        LARGE_INTEGER freq, now;
        QueryPerformanceFrequency(&freq);
        QueryPerformanceCounter(&now);
        return (now.QuadPart / freq.QuadPart) * NANOS_PER_SECOND +
            ((now.QuadPart % freq.QuadPart) * NANOS_PER_SECOND) / freq.QuadPart;
    #else
        struct timespec ts;
        clock_gettime(CLOCK_MONOTONIC, &ts);
        return wssize_t(ts.tv_sec) * NANOS_PER_SECOND + ts.tv_nsec;
    #endif /* PLATFORM_WINDOWS */
    }
} /* namespace */

UTEST_BEGIN("runtime.ipc", thread_sleep)

    UTEST_TIMELIMIT(30)

    class SleepThread: public ipc::Thread
    {
        public:
            wsize_t                 nInterval;      // Sleep interval, milliseconds
            wssize_t                nAlign;         // Start of the sleep within a second of the monotonic clock, milliseconds
            int                     nEntered;       // Set just before the call of sleep()
            wssize_t                nStart;         // Start of the sleep, nanoseconds
            wssize_t                nEnd;           // End of the sleep, nanoseconds
            ssize_t                 nSwitches;
            status_t                nStatus;

        public:
            explicit SleepThread(wsize_t interval, wssize_t align = -1)
            {
                nInterval   = interval;
                nAlign      = align;
                nEntered    = 0;
                nStart      = 0;
                nEnd        = 0;
                nSwitches   = -1;
                nStatus     = STATUS_OK;
            }

            static ssize_t voluntary_switches()
            {
                ssize_t result = -1;
            #if defined(PLATFORM_LINUX)
                char path[64], line[128];
                snprintf(path, sizeof(path), "/proc/self/task/%lu/status", (unsigned long)(current_thread_id()));
                FILE *fd = fopen(path, "r");
                if (fd == NULL)
                    return result;
                while (fgets(line, sizeof(line), fd) != NULL)
                {
                    if (strncmp(line, "voluntary_ctxt_switches:", 24) == 0)
                    {
                        result = atol(&line[24]);
                        break;
                    }
                }
                fclose(fd);
            #endif /* PLATFORM_LINUX */
                return result;
            }

            virtual status_t run()
            {
                // Wait until the monotonic clock is in the window [nAlign, nAlign + 50) ms of a second
                while (nAlign >= 0)
                {
                    const wssize_t millis   = (monotonic_nanos() % NANOS_PER_SECOND) / NANOS_PER_MILLI;
                    const wssize_t delay    = (nAlign + 1000 - millis) % 1000;
                    if (delay == 0)
                        break;
                    if (delay > 950)
                        break;
                    sleep(delay);
                }

                const ssize_t before    = voluntary_switches();
                nStart      = monotonic_nanos();
                atomic_store(&nEntered, 1);
                nStatus     = sleep(nInterval);
                nEnd        = monotonic_nanos();
                const ssize_t after     = voluntary_switches();
                if ((before >= 0) && (after >= 0))
                    nSwitches   = after - before;
                return STATUS_OK;
            }
    };

    UTEST_MAIN
    {
        // Cancel the sleeping thread. With the sleep split into 100 ms intervals, the thread
        // would wake up 70..80 ms after cancel() for these offsets. The 40 ms limit of the
        // wake-up leaves room for the scheduling delays of a loaded system or of valgrind.
        static const wsize_t intervals[]    = { 10000, 10000, ~wsize_t(0) };
        static const wsize_t offsets[]      = { 20, 30, 130 };
        static const wssize_t max_latency   = 40 * NANOS_PER_MILLI;
        for (size_t i=0; i<3; ++i)
        {
            SleepThread t(intervals[i]);
            UTEST_ASSERT(t.start() == STATUS_OK);
            while (atomic_load(&t.nEntered) == 0)
                ipc::Thread::sleep(1);
            ipc::Thread::sleep(offsets[i]);
            UTEST_ASSERT(!t.finished());

            const wssize_t cancel   = monotonic_nanos();
            UTEST_ASSERT(t.cancel() == STATUS_OK);
            UTEST_ASSERT(t.join() == STATUS_OK);
            const wssize_t joined   = monotonic_nanos();

            printf("Cancelled sleep: interval=%llu ms, status=%d, slept before cancel=%.3f ms, wake-up after cancel=%.3f ms, join after cancel=%.3f ms\n",
                (unsigned long long)intervals[i], int(t.nStatus),
                double(cancel - t.nStart) / NANOS_PER_MILLI,
                double(t.nEnd - cancel) / NANOS_PER_MILLI,
                double(joined - cancel) / NANOS_PER_MILLI);
            UTEST_ASSERT(t.nStatus == STATUS_CANCELLED);
            // The thread was inside sleep() when cancelled
            UTEST_ASSERT(cancel - t.nStart >= wssize_t(offsets[i]) * NANOS_PER_MILLI);
            UTEST_ASSERT(t.nEnd >= cancel);
            UTEST_ASSERT(t.nEnd - cancel <= max_latency);
        }

        // Sleep without cancel() should last the whole interval without waking up
        {
            SleepThread t(320);
            UTEST_ASSERT(t.start() == STATUS_OK);
            UTEST_ASSERT(t.join() == STATUS_OK);

            const wssize_t elapsed = t.nEnd - t.nStart;
            printf("Full sleep: status=%d, slept=%.3f ms, voluntary context switches=%d\n",
                int(t.nStatus), double(elapsed) / NANOS_PER_MILLI, int(t.nSwitches));
            UTEST_ASSERT(t.nStatus == STATUS_OK);
            UTEST_ASSERT(elapsed >= 320 * NANOS_PER_MILLI);
        #if defined(PLATFORM_LINUX)
            if (t.nSwitches >= 0)
                UTEST_ASSERT(t.nSwitches <= 2);
            else
                printf("Voluntary context switch check skipped: /proc not readable\n");
        #endif /* PLATFORM_LINUX */
        }

        // Sleep that ends in the next second of the monotonic clock (with the nanosecond carry)
        // and in the same second (without it)
        static const wssize_t aligns[]      = { 900, 100 };
        for (size_t i=0; i<2; ++i)
        {
            SleepThread t(150, aligns[i]);
            UTEST_ASSERT(t.start() == STATUS_OK);
            UTEST_ASSERT(t.join() == STATUS_OK);

            const wssize_t start    = (t.nStart % NANOS_PER_SECOND) / NANOS_PER_MILLI;
            const wssize_t elapsed  = t.nEnd - t.nStart;
            printf("Aligned sleep: status=%d, start=%d ms of the second, slept=%.3f ms\n",
                int(t.nStatus), int(start), double(elapsed) / NANOS_PER_MILLI);
            UTEST_ASSERT(t.nStatus == STATUS_OK);
            UTEST_ASSERT(elapsed >= 150 * NANOS_PER_MILLI);
            UTEST_ASSERT(elapsed <= 250 * NANOS_PER_MILLI);
        #if !defined(PLATFORM_WINDOWS)
            UTEST_ASSERT((start >= aligns[i]) && (start < aligns[i] + 50));
        #endif /* PLATFORM_WINDOWS */
        }
    }

UTEST_END
