/*
 * Copyright (C) 2025 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2025 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-runtime-lib
 * Created on: 24 апр. 2024 г.
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
#include <lsp-plug.in/ipc/Mutex.h>
#include <lsp-plug.in/ipc/SharedMutex.h>

#include <stdarg.h>

using namespace lsp;

UTEST_BEGIN("runtime.ipc", shmutex)

    typedef struct context_t
    {
        ipc::Mutex lock;
        LSPString data;
        uatomic_t sequence_latch;
        size_t errors;

        void log(status_t code, const char *event, status_t expected)
        {
            lock.lock();
            lsp_finally { lock.unlock(); };

            data.append_ascii(event);
            data.append('=');
            if (code == expected)
                data.append_ascii("true");
            else
            {
                data.fmt_append_ascii("false(code=%d)", int(code));
                ++errors;
            }
            data.append(';');
        }
    } context_t;

    static status_t thread_func1(void *arg)
    {
        ipc::SharedMutex mutex;
        context_t *ctx = static_cast<context_t *>(arg);

        wait_latch(ctx->sequence_latch, 0);
        ctx->log(mutex.open("test-lsp.lock"), "open1", STATUS_OK);
        atomic_add(&ctx->sequence_latch, 1); // 0 -> 1

        // Sleep 500 ms and lock the mutex
        wait_latch(ctx->sequence_latch, 3);
        ipc::Thread::sleep(500);
        ctx->log(mutex.lock(), "SYNC1.lock", STATUS_OK);
        atomic_add(&ctx->sequence_latch, 1); // 3 -> 6

        // Sleep 200 ms and unlock the mutex
        wait_latch(ctx->sequence_latch, 7);
        ipc::Thread::sleep(200);
        ctx->log(mutex.unlock(), "SYNC2.unlock", STATUS_OK);
        atomic_add(&ctx->sequence_latch, 1); // 7 -> 9

        // Close the mutex
        wait_latch(ctx->sequence_latch, 10);
        ctx->log(mutex.close(), "close1", STATUS_OK);
        atomic_add(&ctx->sequence_latch, 1); // 10 -> 11

        return STATUS_OK;
    }

    static status_t thread_func2(void *arg)
    {
        ipc::SharedMutex mutex;
        context_t *ctx = static_cast<context_t *>(arg);

        wait_latch(ctx->sequence_latch, 1);
        ctx->log(mutex.open("test-lsp.lock"), "open2", STATUS_OK);
        atomic_add(&ctx->sequence_latch, 1); // 1 -> 2

        // Lock mutex immediately, sleep 500 ms and unlock it
        wait_latch(ctx->sequence_latch, 3);
        ctx->log(mutex.lock(), "lock2", STATUS_OK);
        ipc::Thread::sleep(500);
        ctx->log(mutex.unlock(), "SYNC1.unlock", STATUS_OK);
        atomic_add(&ctx->sequence_latch, 1); // 3 -> 6

        // Call mutltiple times lock, succeed on last call
        wait_latch(ctx->sequence_latch, 6);
        ctx->log(mutex.try_lock(), "trylock2", STATUS_RETRY);
        ctx->log(mutex.lock(500), "timedlock2", STATUS_TIMED_OUT);
        atomic_add(&ctx->sequence_latch, 1); // 6 -> 7

        // Succeed on timed mutex lock
        wait_latch(ctx->sequence_latch, 7);
        ctx->log(mutex.lock(800), "SYNC2.lock", STATUS_OK);
        atomic_add(&ctx->sequence_latch, 1); // 7 -> 9

        // Sleep 200 milliseconds and unlock the mutex
        wait_latch(ctx->sequence_latch, 9);
        ipc::Thread::sleep(200);
        ctx->log(mutex.unlock(), "SYNC3.unlock", STATUS_OK);

        // Close the mutex
        wait_latch(ctx->sequence_latch, 11);
        ctx->log(mutex.close(), "close2", STATUS_OK);
        atomic_add(&ctx->sequence_latch, 1); // 11 -> 12

        return STATUS_OK;
    }

    static void wait_latch(uatomic_t & latch, uatomic_t value)
    {
        while (atomic_load(&latch) != value)
            ipc::Thread::yield();
    }

    void test_simple()
    {
        ipc::SharedMutex mutex;

        printf("Testing simple single-threaded mutex locks\n");

        UTEST_ASSERT(mutex.lock() == STATUS_CLOSED);
        UTEST_ASSERT(mutex.lock(200) == STATUS_CLOSED);
        UTEST_ASSERT(mutex.try_lock() == STATUS_CLOSED);
        UTEST_ASSERT(mutex.unlock() == STATUS_CLOSED);
        UTEST_ASSERT(mutex.close() == STATUS_OK);

        UTEST_ASSERT(mutex.open("test-lsp.lock") == STATUS_OK);
        UTEST_ASSERT(mutex.unlock() == STATUS_BAD_STATE);

        UTEST_ASSERT(mutex.lock() == STATUS_OK);
        UTEST_ASSERT(mutex.lock() == STATUS_LOCKED);
        UTEST_ASSERT(mutex.unlock() == STATUS_OK);
        UTEST_ASSERT(mutex.unlock() == STATUS_BAD_STATE);

        UTEST_ASSERT(mutex.lock(200) == STATUS_OK);
        UTEST_ASSERT(mutex.lock(200) == STATUS_LOCKED);
        UTEST_ASSERT(mutex.unlock() == STATUS_OK);
        UTEST_ASSERT(mutex.unlock() == STATUS_BAD_STATE);

        UTEST_ASSERT(mutex.try_lock() == STATUS_OK);
        UTEST_ASSERT(mutex.try_lock() == STATUS_LOCKED);
        UTEST_ASSERT(mutex.unlock() == STATUS_OK);
        UTEST_ASSERT(mutex.unlock() == STATUS_BAD_STATE);

        UTEST_ASSERT(mutex.close() == STATUS_OK);
    }

    void test_multithreaded()
    {
        ipc::SharedMutex mutex;
        context_t ctx;
        atomic_store(&ctx.sequence_latch, uatomic_t(0));
        ctx.errors = 0;

        printf("Testing simple multi-threaded mutex locks\n");

        ctx.log(mutex.open("test-lsp.lock"), "open", STATUS_OK);
        ctx.log(mutex.lock(), "lock", STATUS_OK);

        ipc::Thread t1(thread_func1, &ctx);
        ipc::Thread t2(thread_func2, &ctx);

        ctx.log(STATUS_OK, "start", STATUS_OK);
        t1.start();
        t2.start();
        wait_latch(ctx.sequence_latch, 2);
        atomic_add(&ctx.sequence_latch, 1); // 2 -> 3

        // Sleep 200 ms and unlock the mutex
        ctx.log(STATUS_OK, "sleep", STATUS_OK);
        ipc::Thread::sleep(200);
        ctx.log(STATUS_OK, "unlock", STATUS_OK);
        UTEST_ASSERT(mutex.unlock() == STATUS_OK);
        atomic_add(&ctx.sequence_latch, 1); // 3 -> 6

        // Lock the mutex, unlock it and return result
        wait_latch(ctx.sequence_latch, 9);
        ctx.log(mutex.lock(), "SYNC3.lock", STATUS_OK);
        ctx.log(mutex.unlock(), "unlock", STATUS_OK);

        ctx.log(mutex.close(), "close", STATUS_OK);
        atomic_add(&ctx.sequence_latch, 1); // 9 -> 10

        // Do final comparison
        wait_latch(ctx.sequence_latch, 12);

        printf("Result sequence:    %s\n", ctx.data.get_ascii());
        UTEST_ASSERT(ctx.errors == 0);
    }

    UTEST_MAIN
    {
        test_simple();
        test_multithreaded();
    }
UTEST_END;



