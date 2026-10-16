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

#include <lsp-plug.in/common/types.h>
#include <lsp-plug.in/common/atomic.h>
#include <lsp-plug.in/ipc/Thread.h>

#include <time.h>
#include <errno.h>
#include <unistd.h>

#if defined(PLATFORM_WINDOWS)
    #include <windows.h>
    #include <processthreadsapi.h>
    #include <synchapi.h>
#else
    #include <pthread.h>
#endif /* PLATFORM_WINDOWS */

#if defined(PLATFORM_WINDOWS)
    // Nothing
#elif defined(PLATFORM_LINUX)
    #include <sched.h>
    #include <sys/syscall.h>
#elif defined(PLATFORM_SOLARIS)
    #include <pthread.h>
#elif defined(PLATFORM_FREEBSD)
    #include <sched.h>
    #include <sys/thr.h>
#elif defined(PLATFORM_NETBSD)
    #include <sched.h>
    #include <lwp.h>
#elif defined(PLATFORM_MACOSX)
    #include <mach/mach.h>
    #include <mach/mach_error.h>
    #include <sched.h>
#elif defined(PLATFORM_DRAGONFLYBSD)
    #include <sched.h>
    #include <lwp.h>
#elif defined(PLATFORM_HAIKU)
    #include <kernel/OS.h>
#endif /* PLATFORM_WINDOWS */

namespace lsp
{
    namespace ipc
    {
        namespace detail
        {
            struct thread_impl_t
            {
            #if defined(PLATFORM_WINDOWS)
                HANDLE                      hThread;        // Windows threads
                CRITICAL_SECTION            sLock;          // Lock for cancellation wake-up
                CONDITION_VARIABLE          sCancel;        // Cancellation wake-up for sleep()

                static DWORD WINAPI         launcher(_In_ LPVOID lpParameter);
            #else
                pthread_t                   hThread;        // POSIX threads
                pthread_mutex_t             sLock;          // Lock for cancellation wake-up
                pthread_cond_t              sCancel;        // Cancellation wake-up for sleep()
                clockid_t                   nClockId;       // Clock used by sleep()

                static void                *launcher(void *arg);
            #endif /* PLATFORM_WINDOWS */
            };
        } /* namespace detail */

        __thread Thread *Thread::pThis = NULL;

    #if !defined(PLATFORM_WINDOWS)
        static void add_millis(struct timespec *ts, wsize_t millis)
        {
            const time_t max_sec = time_t((~uint64_t(0)) >> (65 - sizeof(time_t) * 8)); // Maximum value of time_t
            const uint64_t sec  = millis / 1000;
            ts->tv_nsec        += (millis % 1000) * 1000000;
            ts->tv_sec         += ts->tv_nsec / 1000000000;
            ts->tv_nsec        %= 1000000000;

            // Wait until cancelled if the deadline does not fit into time_t
            if (sec > uint64_t(max_sec - ts->tv_sec))
            {
                ts->tv_sec          = max_sec;
                ts->tv_nsec         = 999999999;
            }
            else
                ts->tv_sec         += sec;
        }
    #endif /* PLATFORM_WINDOWS */

    #if defined(PLATFORM_MACOSX)
        // Convert the current time to the time left until the deadline, false if it has been reached
        static bool time_left(struct timespec *left, const struct timespec *deadline)
        {
            if ((left->tv_sec > deadline->tv_sec) ||
                ((left->tv_sec == deadline->tv_sec) && (left->tv_nsec >= deadline->tv_nsec)))
                return false;

            left->tv_sec        = deadline->tv_sec - left->tv_sec;
            left->tv_nsec       = deadline->tv_nsec - left->tv_nsec;
            if (left->tv_nsec < 0)
            {
                left->tv_nsec      += 1000000000;
                --left->tv_sec;
            }

            // Limit the relative wait to one day, the deadline is checked again after it
            if (left->tv_sec >= 86400)
                left->tv_sec        = 86400;
            return true;
        }
    #endif /* PLATFORM_MACOSX */

        Thread::Thread()
        {
            atomic_store(&enState, TS_CREATED);
            nResult             = STATUS_OK;
            bCancelled          = false;
            init_impl();
            sBinding.proc       = NULL;
            sBinding.arg        = NULL;
            sBinding.runnable   = NULL;
        }
        
        Thread::Thread(thread_proc_t proc)
        {
            atomic_store(&enState, TS_CREATED);
            nResult             = STATUS_OK;
            bCancelled          = false;
            init_impl();
            sBinding.proc       = proc;
            sBinding.arg        = NULL;
            sBinding.runnable   = NULL;
        }

        Thread::Thread(thread_proc_t proc, void *arg)
        {
            atomic_store(&enState, TS_CREATED);
            nResult             = STATUS_OK;
            bCancelled          = false;
            init_impl();
            sBinding.proc       = proc;
            sBinding.runnable   = NULL;
            sBinding.arg        = arg;
        }

        Thread::Thread(IRunnable *runnable)
        {
            atomic_store(&enState, TS_CREATED);
            nResult             = STATUS_OK;
            bCancelled          = false;
            init_impl();
            sBinding.proc       = NULL;
            sBinding.arg        = NULL;
            sBinding.runnable   = runnable;
        }

        Thread::~Thread()
        {
            if (pImpl == NULL)
                return;

    #if defined(PLATFORM_WINDOWS)
            if (pImpl->hThread != INVALID_HANDLE_VALUE)
                CloseHandle(pImpl->hThread);
            DeleteCriticalSection(&pImpl->sLock);
    #else
            pthread_cond_destroy(&pImpl->sCancel);
            pthread_mutex_destroy(&pImpl->sLock);
    #endif /* PLATFORM_WINDOWS */

            delete pImpl;
            pImpl               = NULL;
        }

        void Thread::init_impl()
        {
            pImpl               = new detail::thread_impl_t;
            if (pImpl == NULL)
                return;

    #if defined(PLATFORM_WINDOWS)
            pImpl->hThread      = INVALID_HANDLE_VALUE;
            InitializeCriticalSection(&pImpl->sLock);
            InitializeConditionVariable(&pImpl->sCancel);
    #else
            pthread_mutex_init(&pImpl->sLock, NULL);

            // Use CLOCK_MONOTONIC for sleep() if possible, CLOCK_REALTIME otherwise. MacOS has no
            // pthread_condattr_setclock() and waits for the time left until CLOCK_MONOTONIC deadline.
            pthread_condattr_t attr;
            pthread_condattr_init(&attr);
            pImpl->nClockId     = CLOCK_MONOTONIC;
        #if !defined(PLATFORM_MACOSX)
            if (pthread_condattr_setclock(&attr, CLOCK_MONOTONIC) != 0)
                pImpl->nClockId     = CLOCK_REALTIME;
        #endif /* PLATFORM_MACOSX */
            pthread_cond_init(&pImpl->sCancel, &attr);
            pthread_condattr_destroy(&attr);
    #endif /* PLATFORM_WINDOWS */
        }

        status_t Thread::run()
        {
            if (sBinding.proc != NULL)
                return sBinding.proc(sBinding.arg);
            else if (sBinding.runnable != NULL)
                return sBinding.runnable->run();
            return STATUS_OK;
        }

        status_t Thread::cancel()
        {
            switch (atomic_load(&enState))
            {
                case TS_PENDING:
                case TS_RUNNING:
                    break;
                default:
                    return STATUS_BAD_STATE;
            }

            // Set the flag and wake up the thread if it sleeps. The thread has been started,
            // so pImpl is not NULL.
    #if defined(PLATFORM_WINDOWS)
            EnterCriticalSection(&pImpl->sLock);
            bCancelled  = true;
            WakeConditionVariable(&pImpl->sCancel);
            LeaveCriticalSection(&pImpl->sLock);
    #else
            pthread_mutex_lock(&pImpl->sLock);
            bCancelled  = true;
            pthread_cond_signal(&pImpl->sCancel);
            pthread_mutex_unlock(&pImpl->sLock);
    #endif /* PLATFORM_WINDOWS */

            return STATUS_OK;
        }
    
    #if defined(PLATFORM_WINDOWS)
        DWORD WINAPI detail::thread_impl_t::launcher(_In_ LPVOID lpParameter)
        {
            Thread *_this = reinterpret_cast<Thread *>(lpParameter);
            Thread::pThis   = _this;

            // Wait until we are ready to launch
            while (!atomic_cas(&_this->enState, TS_PENDING, TS_RUNNING)) {}

            // Execute the thread
            status_t res    = _this->run();
            Thread::pThis   = NULL;

            // Commit the 'FINISHED' status
            int state;
            do
            {
                state       = atomic_load(&_this->enState);
            } while (!atomic_cas(&_this->enState, state, TS_FINISHED));

            _this->nResult  = res;
            return 0;
        }

        status_t Thread::start()
        {
            if (pImpl == NULL)
                return STATUS_NO_MEM;

            DWORD tid;
            HANDLE thandle = CreateThread(NULL, 0, detail::thread_impl_t::launcher, this, 0, &tid);
            if (thandle == INVALID_HANDLE_VALUE)
                return STATUS_UNKNOWN_ERR;

            pImpl->hThread  = thandle;
            atomic_store(&enState, TS_PENDING);
            return STATUS_OK;
        }

        status_t Thread::join()
        {
            switch (atomic_load(&enState))
            {
                case TS_CREATED:
                    return STATUS_BAD_STATE;
                case TS_PENDING:
                case TS_RUNNING:
                {
                    DWORD res = WaitForSingleObject(pImpl->hThread, INFINITE);
                    if ((res != WAIT_OBJECT_0) && (res != WAIT_ABANDONED))
                        return STATUS_UNKNOWN_ERR;
                    break;
                }
                case TS_FINISHED:
                    return STATUS_OK;
                default:
                    return STATUS_BAD_STATE;
            }
            return STATUS_OK;
        }

        status_t Thread::sleep(wsize_t millis)
        {
            if (pThis == NULL)
            {
                while (millis > 0)
                {
                    DWORD interval  = (millis > 500) ? 500 : millis;
                    Sleep(interval);
                    millis         -= interval;
                }
            }
            else
            {
                if (pThis->bCancelled)
                    return STATUS_CANCELLED;

                // Wait until the timeout expires or the thread is cancelled
                Thread *self        = pThis;
                detail::thread_impl_t *impl = self->pImpl;
                status_t res        = STATUS_OK;
                wsize_t left        = millis;
                LARGE_INTEGER freq, start, now;
                QueryPerformanceFrequency(&freq);
                QueryPerformanceCounter(&start);

                EnterCriticalSection(&impl->sLock);
                while ((!self->bCancelled) && (left > 0))
                {
                    const DWORD timeout = (left >= INFINITE) ? INFINITE - 1 : DWORD(left);
                    if ((!SleepConditionVariableCS(&impl->sCancel, &impl->sLock, timeout)) &&
                        (GetLastError() != ERROR_TIMEOUT))
                    {
                        res                 = STATUS_UNKNOWN_ERR;
                        break;
                    }

                    // Compute the time left, the timeout may expire early and wake-ups may be spurious
                    QueryPerformanceCounter(&now);
                    const wsize_t passed = wsize_t(now.QuadPart - start.QuadPart) * 1000 / wsize_t(freq.QuadPart);
                    left                = (passed < millis) ? millis - passed : 0;
                }
                if (self->bCancelled)
                    res                 = STATUS_CANCELLED;
                LeaveCriticalSection(&impl->sLock);

                return res;
            }

            return STATUS_OK;
        }

        void Thread::yield()
        {
            SwitchToThread();
        }

    #else
        void *detail::thread_impl_t::launcher(void *arg)
        {
            Thread *_this   = reinterpret_cast<Thread *>(arg);
            Thread::pThis   = _this;

            // Cleanup cancellation state
            pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
            pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL);

            // Wait until we are ready to launch
            while (!atomic_cas(&_this->enState, TS_PENDING, TS_RUNNING)) {}

            // Execute the thread
            status_t res    = _this->run();

            // Commit the 'FINISHED' status
            int state;
            do
            {
                state       = atomic_load(&_this->enState);
            } while (!atomic_cas(&_this->enState, state, TS_FINISHED));

            _this->nResult  = res;
            return NULL;
        }

        status_t Thread::start()
        {
            if (pImpl == NULL)
                return STATUS_NO_MEM;

            pthread_t tid;
            if (pthread_create(&tid, NULL, &detail::thread_impl_t::launcher, this) != 0)
                return STATUS_UNKNOWN_ERR;

            pImpl->hThread  = tid;
            atomic_store(&enState, TS_PENDING);
            return STATUS_OK;
        }

        status_t Thread::join()
        {
            switch (atomic_load(&enState))
            {
                case TS_CREATED:
                    return STATUS_BAD_STATE;
                case TS_PENDING:
                case TS_RUNNING:
                    if (pthread_join(pImpl->hThread, NULL) != 0)
                        return STATUS_UNKNOWN_ERR;
                    break;
                case TS_FINISHED:
                    return STATUS_OK;
                default:
                    return STATUS_BAD_STATE;
            }
            return STATUS_OK;
        }

        status_t Thread::sleep(wsize_t millis)
        {
            struct timespec req, rem;

            if (pThis == NULL)
            {
                req.tv_sec  = millis / 1000;
                req.tv_nsec = (millis % 1000) * 1000000;

                while (::nanosleep(&req, &rem) != 0)
                {
                    int code = errno;
                    if (code != EINTR)
                        return STATUS_UNKNOWN_ERR;
                    req = rem;
                }
            }
            else
            {
                if (pThis->bCancelled)
                    return STATUS_CANCELLED;

                // Compute the deadline
                Thread *self        = pThis;
                detail::thread_impl_t *impl = self->pImpl;
                status_t res        = STATUS_OK;
                struct timespec deadline;

                if (::clock_gettime(impl->nClockId, &deadline) != 0)
                    return STATUS_UNKNOWN_ERR;
                add_millis(&deadline, millis);

                // Wait until the deadline is reached or the thread is cancelled
                pthread_mutex_lock(&impl->sLock);
                while (!self->bCancelled)
                {
        #if defined(PLATFORM_MACOSX)
                    struct timespec left;
                    if (::clock_gettime(CLOCK_MONOTONIC, &left) != 0)
                    {
                        res                 = STATUS_UNKNOWN_ERR;
                        break;
                    }
                    if (!time_left(&left, &deadline))
                        break;
                    const int code      = pthread_cond_timedwait_relative_np(&impl->sCancel, &impl->sLock, &left);
                    if ((code != 0) && (code != ETIMEDOUT))
                    {
                        res                 = STATUS_UNKNOWN_ERR;
                        break;
                    }
        #else
                    const int code      = pthread_cond_timedwait(&impl->sCancel, &impl->sLock, &deadline);
                    if (code == ETIMEDOUT)
                        break;
                    else if (code != 0)
                    {
                        res                 = STATUS_UNKNOWN_ERR;
                        break;
                    }
        #endif /* PLATFORM_MACOSX */
                }
                if (self->bCancelled)
                    res                 = STATUS_CANCELLED;
                pthread_mutex_unlock(&impl->sLock);

                return res;
            }

            return STATUS_OK;
        }

        void Thread::yield()
        {
            sched_yield();
        }

    #endif /* PLATFORM_WINDOWS */


        thread_id_t Thread::current_thread_id()
        {
            thread_id_t result      = INVALID_THREAD_ID;

        #if defined(PLATFORM_WINDOWS)
            return GetCurrentThreadId();
        #elif defined(PLATFORM_LINUX)
            result                  = syscall( __NR_gettid );
        #elif defined(PLATFORM_SOLARIS)
            result                  = pthread_self();
        #elif defined(PLATFORM_MACOSX)
            const mach_port_t port  = mach_thread_self();
            mach_port_deallocate(mach_task_self(), port);
            result                  = port;
        #elif defined(PLATFORM_NETBSD)
            result                  = _lwp_self();
        #elif defined(PLATFORM_FREEBSD)
            long lwpid;
            thr_self( &lwpid );
            result                  = lwpid;
        #elif defined(PLATFORM_DRAGONFLYBSD)
            result                  = lwp_gettid();
        #elif defined (PLATFORM_HAIKU)
            result                  = find_thread(NULL);
        #else
            #error "need to implement Thread::current_thread_id"
        #endif
            return result;
        }

    } /* namespace ipc */
} /* namespace lsp */
