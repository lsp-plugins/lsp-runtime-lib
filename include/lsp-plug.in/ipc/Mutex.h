/*
 * Copyright (C) 2024 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2024 Vladimir Sadovnikov <sadko4u@gmail.com>
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

#ifndef LSP_PLUG_IN_IPC_MUTEX_H_
#define LSP_PLUG_IN_IPC_MUTEX_H_

#include <lsp-plug.in/runtime/version.h>
#include <lsp-plug.in/common/types.h>
#include <lsp-plug.in/common/atomic.h>

#if defined(PLATFORM_WINDOWS)
    #include <synchapi.h>
#elif defined(PLATFORM_LINUX)
    #include <linux/futex.h>
    #include <sys/syscall.h>
    #include <unistd.h>
    #include <pthread.h>
    #include <sched.h>
    #include <errno.h>
#else
    #include <pthread.h>
    #include <sched.h>
    #include <errno.h>
#endif

namespace lsp
{
    namespace ipc
    {
        /**
         * Recursive mutex for Windows platform
         */
        class Mutex
        {
            private:
        #if defined(PLATFORM_WINDOWS)
                mutable HANDLE                  hMutex;     // Mutex object
                mutable DWORD                   nThreadId;  // Owner's thread identifier
                mutable atomic_t                nLocks;     // Number of locks by current thread
        #elif defined(PLATFORM_LINUX)
                mutable volatile int            nLock;      // 1 = unlocked, 0 = locked
                mutable volatile pthread_t      nThreadId;  // Locked thread identifier
                mutable atomic_t                nLocks;     // Number of locks by current thread
                mutable Mutex                  *pNext;      // Overall number of waiters
        #else
                mutable pthread_mutex_t         sMutex;
        #endif

            public:
                Mutex();
                Mutex(const Mutex &) = delete;
                Mutex(Mutex &&) = delete;

                Mutex & operator = (const Mutex &) = delete;
                Mutex & operator = (Mutex &&) = delete;

            public:
                /**
                 * Wait until mutex becomes unlocked and lock it
                 * @return status of operation (may be false for non-recursive mutexes)
                 */
                bool lock() const;

                /**
                 * Try to lock mutex and return status of operation
                 * @return true if mutex was locked
                 */
                bool try_lock() const;

                /**
                 * Unlock mutex
                 * @return true if mutex has been unlocked
                 */
                bool unlock() const;

                /**
                 * Wait until someone else issues notification on the mutex
                 * @return false if wait is not possible (for example, when the mutex is not locked)
                 */
                bool wait() const;

                /**
                 * Wait for the specified period of time until someone else issues notification the mutex
                 * @param millis time period to wait in milliseconds
                 * @return false if wait is not possible (for example, when the mutex is not locked)
                 */
                bool wait(wsize_t millis) const;

                /**
                 * Send notification to random thead that is waiting the mutex
                 * @return false if current thread is not owning the mutex
                 */
                bool notify() const;

                /**
                 * Send notification to all threads that are waiting on the mutex
                 * @return false if current thread is not owning the mutex
                 */
                bool notify_all() const;
        };
    
    } /* namespace ipc */
} /* namespace lsp */

#endif /* LSP_PLUG_IN_IPC_MUTEX_H_ */
