/*
 * IRunnable.h
 *
 *  Created on: 5 мар. 2019 г.
 *      Author: sadko
 */

#ifndef LSP_PLUG_IN_IPC_IRUNNABLE_H_
#define LSP_PLUG_IN_IPC_IRUNNABLE_H_

#include <lsp-plug.in/runtime/version.h>
#include <lsp-plug.in/common/status.h>

namespace lsp
{
    namespace ipc
    {
        /**
         * Runnable interface
         */
        class IRunnable
        {
            private:
                IRunnable & operator = (const IRunnable &src);      // Deny copying

            public:
                explicit IRunnable();
                virtual ~IRunnable();

            public:
                /**
                 * The main method of the runnable interface
                 * @return status of operation
                 */
                virtual status_t run();
        };
    
    } /* namespace ipc */
} /* namespace lsp */

#endif /* LSP_PLUG_IN_IPC_IRUNNABLE_H_ */
