/*
 * Copyright (C) 2026 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2026 Vladimir Sadovnikov <sadko4u@gmail.com>
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

#ifndef LSP_PLUG_IN_IPC_THREAD_H_
#define LSP_PLUG_IN_IPC_THREAD_H_

#include <lsp-plug.in/runtime/version.h>
#include <lsp-plug.in/common/atomic.h>
#include <lsp-plug.in/common/types.h>
#include <lsp-plug.in/common/status.h>

#include <lsp-plug.in/ipc/IRunnable.h>
#include <lsp-plug.in/runtime/system.h>

namespace lsp
{
    namespace ipc
    {
        enum thread_state_t
        {
            TS_CREATED,
            TS_STARTING,
            TS_PENDING,
            TS_RUNNING,
            TS_FINISHED,
            TS_UNKNOWN,
        };

        /**
         * Thread procedure that can be launched
         * @param arg additional argument passed to a thread procedure
         * @return status of thread execution
         */
        typedef status_t (* thread_proc_t)(void *arg);

        typedef umword_t                thread_id_t;

        constexpr thread_id_t INVALID_THREAD_ID     = 0;

        namespace detail
        {
            struct thread_impl_t;
        } /* namespace detail */

        /**
         * Thread class
         */
        class Thread: public IRunnable
        {
            private:
                static __thread Thread     *pThis;
                detail::thread_impl_t      *pImpl;          // Platform-specific thread handle and cancellation wake-up

            private:
                friend struct detail::thread_impl_t;

                void                        init_impl();
                void                        release_impl();

            public:
                explicit Thread();
                explicit Thread(thread_proc_t proc);
                explicit Thread(thread_proc_t proc, void *arg);
                explicit Thread(IRunnable *runnable);

                Thread(const Thread & src) = delete;
                Thread(Thread && src) = delete;
                Thread & operator = (const Thread & src) = delete;
                Thread & operator = (Thread && src) = delete;

                virtual ~Thread();

            public:
                /**
                 * The thread's main execution method
                 * @return status of thread execution
                 */
                virtual status_t run();

                /**
                 * Launch the created thread
                 * @return status of operation
                 */
                status_t start();

                /**
                 * Send cancel request to the thread
                 * @return status of operation
                 */
                status_t cancel();

                /**
                 * Wait thread for completion
                 * @return status of operation
                 */
                status_t join();

                /**
                 * Force current thread to leep for amount of milliseconds
                 * @param millis interval in milliseconds to perform a sleep
                 */
                static status_t sleep(wsize_t millis);

                /**
                 * Causes the calling thread to yield execution to another thread that
                 * is ready to run on the current processor.
                 */
                static void yield();

                /**
                 * Return the current thread
                 * @return current thread or NULL if current thread is not an instance of ipc::Thread class
                 */
                static inline Thread *current()     { return pThis;     }

                /** Check that cancellation signal has been delivered to the thread
                 * @return true if thread has been cancelled
                 */
                bool cancelled() const;

                /** Check that cancellation signal has been delivered to the current thread
                 * @return true if cancellation signal is pending
                 */
                static inline bool is_cancelled()   { return (pThis != NULL) ? pThis->cancelled() : false; };

                /**
                 * Check whether thread has finished
                 * @return true if thread has finished
                 */
                bool finished() const;

                /**
                 * Get thread state
                 * @return thread state
                 */
                thread_state_t state() const;

                /**
                 * Return the execution result of the thread
                 * @return execution result of the thread
                 */
                status_t get_result() const;
                inline status_t result() const      { return get_result();  }

                /**
                 * Return number of execution cores supported by the system
                 * @return number of logical CPUs in the system available for processing
                 */
                static inline size_t system_cores() { return system::system_cores(); }

                /**
                 * Get current thread identifier.
                 * @return current thread identifier
                 */
                static thread_id_t      current_thread_id();
        };
    
    } /* namespace ipc */
} /* namespace lsp */

#endif /* LSP_PLUG_IN_IPC_THREAD_H_ */
