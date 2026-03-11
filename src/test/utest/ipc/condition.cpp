/*
 * Copyright (C) 2026 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2026 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-runtime-lib
 * Created on: 4 янв. 2026 г.
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
#include <lsp-plug.in/ipc/Condition.h>

using namespace lsp;

UTEST_BEGIN("runtime.ipc", condition)

    UTEST_TIMELIMIT(10)

    status_t execute(int id, int *shared, ipc::Condition *condition)
    {
        // Wait for the condition
        {
            UTEST_ASSERT(condition->lock());
            lsp_finally { condition->unlock(); };

            while (*shared != id)
                condition->wait();
        }

        printf("Thread %d has been awaken\n", id);

        // Do some sleep
        ipc::Thread::sleep(500);

        // Trigger condition
        {
            UTEST_ASSERT(condition->lock());
            lsp_finally { condition->unlock(); };

            ++(*shared);
            UTEST_ASSERT(condition->notify_all());
        }

        return 1;
    }

    class TestThread: public ipc::Thread
    {
        private:
            test_type_t *test;
            ipc::Condition *condition;
            int *shared;
            int id;

        public:
            explicit TestThread()
            {
                test = NULL;
                shared = NULL;
                id = 0;
                condition = NULL;
            }
            virtual ~TestThread() {}

            void bind(test_type_t *test, int *shared, int id, ipc::Condition *condition)
            {
                this->test = test;
                this->shared = shared;
                this->id = id;
                this->condition = condition;
            }

            virtual status_t run()
            {
                return test->execute(id, shared, condition);
            }
    };


    UTEST_MAIN
    {
        TestThread t[4];
        ipc::Condition condition;
        int shared = 0;

        printf("Starting threads...\n");
        for (size_t i=0; i<4; ++i)
        {
            t[i].bind(this, &shared, int(i+1), &condition);
            UTEST_ASSERT(t[i].start() == STATUS_OK);
        }

        // Do some sleep
        printf("Performing sleep\n");
        ipc::Thread::sleep(1000);

        printf("Signalling first thread to wake up\n");
        {
            UTEST_ASSERT(condition.lock());
            lsp_finally { condition.unlock(); };
            ++shared;
            UTEST_ASSERT(condition.notify_all());
        }

        printf("Waiting threads for termination...\n");
        for (size_t i=0; i<4; ++i)
            t[i].join();

        printf("Analyzing exit status...\n");
        for (int i=0; i<4; ++i)
        {
            UTEST_ASSERT(!t[i].cancelled());
            UTEST_ASSERT(t[i].get_result() == 1);
        }

        UTEST_ASSERT(shared == 5);
    }
UTEST_END;



