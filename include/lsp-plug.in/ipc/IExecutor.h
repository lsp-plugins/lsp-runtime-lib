/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-runtime-lib
 * Created on: 27 янв. 2016 г.
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

#ifndef LSP_PLUG_IN_IPC_IEXECUTOR_H_
#define LSP_PLUG_IN_IPC_IEXECUTOR_H_

#include <lsp-plug.in/runtime/version.h>
#include <lsp-plug.in/common/types.h>
#include <lsp-plug.in/ipc/ITask.h>

namespace lsp
{
    namespace ipc
    {
        class IExecutor
        {
            protected:
                static inline void change_task_state(ITask *task, ITask::task_state_t state)
                {
                    task->nState    = state;
                }

                static inline void link_task(ITask *tail, ITask *link)
                {
                    tail->pNext     = link;
                    link->pNext     = NULL;
                }

                static inline ITask *next_task(ITask *task)
                {
                    ITask *next     = task->pNext;
                    task->pNext     = NULL;
                    return next;
                }

                static inline void run_task(ITask *task)
                {
                    task->nState    = ITask::TS_RUNNING;
                    task->nCode     = 0;
                    task->nCode     = task->run();
                    task->nState    = ITask::TS_COMPLETED;
                }

            private:
                IExecutor &operator = (const IExecutor &src);       // Deny copying

            public:
                explicit IExecutor();
                virtual ~IExecutor();

            public:
                /** Submit task for execution
                 *
                 * @param task task to execute
                 * @return true if task was submitted
                 */
                virtual bool submit(ITask *task);

                /** Shutdown executor service
                 * The method must return only when all tasks
                 * have been completed or terminated
                 *
                 */
                virtual void shutdown();
        };

    } /* namespace ipc */
} /* namespace lsp */

#endif /* LSP_PLUG_IN_IPC_IEXECUTOR_H_ */
