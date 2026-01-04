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

#include <lsp-plug.in/ipc/Condition.h>

#ifdef PLATFORM_WINDOWS
    #include <windows.h>
    #include <synchapi.h>
#else
    #include <errno.h>
    #include <sys/time.h>
    #include <sched.h>
#endif /* PLATFORM_WINDOWS */

namespace lsp
{
    namespace ipc
    {
#if defined(PLATFORM_WINDOWS)

        namespace detail
        {
            struct CRITICAL_SECTION: public ::CRITICAL_SECTION
            {
            };
        } /* namespace detail */

        Mutex::Mutex()
        {
            hMutex      = new detail::CRITICAL_SECTION;
            if (hMutex != NULL)
                InitializeCriticalSectionAndSpinCount(hMutex, 0x10);
        }

        Mutex::~Mutex()
        {
            if (hMutex != NULL)
            {
                DeleteCriticalSection(hMutex);
                delete hMutex;
            }
        }

        bool Mutex::lock() const
        {
            EnterCriticalSection(hMutex);
            return true;
        }

        bool Mutex::try_lock() const
        {
            return TryEnterCriticalSection(hMutex);
        }

        bool Mutex::unlock() const
        {
            HANDLE thread = reinterpret_cast<HANDLE>(GetCurrentThreadId());
            if (hMutex->OwningThread != thread)
                return false;
            if (hMutex->RecursionCount <= 0)
                return false;

            LeaveCriticalSection(hMutex);
            return true;
        }

#else
        Condition::Condition()
        {
            pthread_mutexattr_t attr;
            pthread_mutexattr_init(&attr);
            pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_DEFAULT);
            pthread_mutex_init(&sMutex, &attr);
            pthread_mutexattr_destroy(&attr);

            pthread_condattr_t cond_attr;
            pthread_condattr_init(&cond_attr);

            pthread_cond_init(&sCondition, &cond_attr);
        }

        Condition::~Condition()
        {
            pthread_cond_destroy(&sCondition);
            pthread_mutex_destroy(&sMutex);
        }

        bool Condition::lock() const
        {
            while (true)
            {
                switch (pthread_mutex_lock(&sMutex))
                {
                    case 0: return true;
                    case EBUSY:
                        sched_yield();
                        break;
                    default: return false;
                }
            }
        }

        status_t Condition::wait(system::time_millis_t millis)
        {
            struct timeval now;
            struct timespec deadline;

            // Set-up the fire time
            gettimeofday(&now, NULL);
            deadline.tv_nsec    = now.tv_usec * 1000 + (millis % 1000) * 1000000;
            deadline.tv_sec     = now.tv_sec + (millis / 1000) + (deadline.tv_nsec / 1000000000);
            deadline.tv_nsec   %= 1000000000;

            // Perform wait
            int result          = pthread_cond_timedwait(&sCondition, &sMutex, &deadline);
            switch (result)
            {
                case 0: return STATUS_OK;
                case ETIMEDOUT: return STATUS_TIMED_OUT;
                case EPERM: return STATUS_BAD_STATE;
                default: break;
            }

            return STATUS_UNKNOWN_ERR;
        }

        status_t Condition::wait()
        {
            int result          = pthread_cond_wait(&sCondition, &sMutex);
            switch (result)
            {
                case 0: return STATUS_OK;
                case ETIMEDOUT: return STATUS_TIMED_OUT;
                case EPERM: return STATUS_BAD_STATE;
                default: break;
            }

            return STATUS_UNKNOWN_ERR;
        }

#endif /* PLATFORM_LINUX */

    } /* namespace ipc */
} /* namespace lsp */



