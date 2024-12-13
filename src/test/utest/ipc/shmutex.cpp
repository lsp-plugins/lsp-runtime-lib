/*
 * Copyright (C) 2024 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2024 Vladimir Sadovnikov <sadko4u@gmail.com>
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

        void log(status_t code, const char *event, status_t expected)
        {
            lock.lock();
            lsp_finally { lock.unlock(); };

            data.append_ascii(event);
            data.append('=');
            data.append_ascii((code == expected) ? "true" : "false");
            data.append(';');
        }
    } context_t;


    static status_t thread_func1(void *arg)
    {
        ipc::SharedMutex mutex;
        context_t *ctx = static_cast<context_t *>(arg);

        ipc::Thread::sleep(500);
        ctx->log(mutex.open("test-lsp.lock"), "open1", STATUS_OK);
        ctx->log(mutex.lock(), "SYNC1", STATUS_OK);
        ipc::Thread::sleep(800);
        ctx->log(mutex.unlock(), "SYNC2", STATUS_OK);

        return STATUS_OK;
    }

    static status_t thread_func2(void *arg)
    {
        ipc::SharedMutex mutex;
        context_t *ctx = static_cast<context_t *>(arg);

        ctx->log(mutex.open("test-lsp.lock"), "open2", STATUS_OK);
        ctx->log(mutex.lock(), "lock2", STATUS_OK);

        ipc::Thread::sleep(500);

        ctx->log(mutex.unlock(), "SYNC1", STATUS_OK);

        ipc::Thread::sleep(100);

        ctx->log(mutex.try_lock(), "trylock2", STATUS_RETRY);
        ctx->log(mutex.lock(500), "timedlock2", STATUS_TIMED_OUT);
        ctx->log(mutex.lock(500), "SYNC2", STATUS_OK);
        ipc::Thread::sleep(200);
        ctx->log(mutex.unlock(), "SYNC3", STATUS_OK);

        return STATUS_OK;
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

        printf("Testing simple multi-threaded mutex locks\n");

        ctx.log(mutex.open("test-lsp.lock"), "open", STATUS_OK);
        ctx.log(mutex.lock(), "lock", STATUS_OK);

        ipc::Thread t1(thread_func1, &ctx);
        ipc::Thread t2(thread_func2, &ctx);

        ctx.log(STATUS_OK, "start", STATUS_OK);
        t1.start();
        t2.start();

        ctx.log(STATUS_OK, "sleep", STATUS_OK);
        ipc::Thread::sleep(200);

        ctx.log(STATUS_OK, "unlock", STATUS_OK);
        UTEST_ASSERT(mutex.unlock() == STATUS_OK);

        ipc::Thread::sleep(2000);
        ctx.log(mutex.lock(), "SYNC3", STATUS_OK);
        ctx.log(mutex.unlock(), "unlock", STATUS_OK);

        ctx.log(mutex.close(), "close", STATUS_OK);

        static const char *expected =
            "open=true;lock=true;start=true;sleep=true;open2=true;unlock=true;lock2=true;open1=true;"
            "SYNC1=true;SYNC1=true;trylock2=true;timedlock2=true;"
            "SYNC2=true;SYNC2=true;"
            "SYNC3=true;SYNC3=true;unlock=true;close=true;";

        printf("Result content:   %s\n", ctx.data.get_ascii());
        printf("Expected content: %s\n", expected);

        UTEST_ASSERT(ctx.data.equals_ascii(expected));
    }

    UTEST_MAIN
    {
        test_simple();
        test_multithreaded();
    }
UTEST_END;



