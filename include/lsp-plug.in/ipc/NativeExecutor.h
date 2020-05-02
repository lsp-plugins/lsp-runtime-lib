/*
 * NativeExecutor.h
 *
 *  Created on: 27 янв. 2016 г.
 *      Author: sadko
 */

#ifndef CORE_NATIVEEXECUTOR_H_
#define CORE_NATIVEEXECUTOR_H_

#include <lsp-plug.in/runtime/version.h>
#include <lsp-plug.in/common/atomic.h>
#include <lsp-plug.in/ipc/Thread.h>
#include <lsp-plug.in/ipc/IExecutor.h>
#include <lsp-plug.in/ipc/ITask.h>

namespace lsp
{
    namespace ipc
    {
        class NativeExecutor: public IExecutor
        {
            private:
                Thread              hThread;
                ITask              *pHead;
                ITask              *pTail;
                atomic_t            nLock;

                static status_t     execute(void *params);
                void    run();

            private:
                NativeExecutor &operator = (const NativeExecutor &src); // Deny copying

            public:
                explicit NativeExecutor();
                virtual ~NativeExecutor();

            public:
                status_t start();

                virtual bool submit(ITask *task);

                virtual void shutdown();
        };
    }
}

#endif /* CORE_NATIVEEXECUTOR_H_ */
