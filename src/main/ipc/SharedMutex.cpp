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

#include <lsp-plug.in/common/debug.h>
#include <lsp-plug.in/ipc/SharedMutex.h>

#ifdef PLATFORM_WINDOWS
    #include <windows.h>
    #include <synchapi.h>
#else
    #include <errno.h>
    #include <fcntl.h>
    #include <sys/stat.h>
    #include <time.h>
#endif /* PLATFORM_WINDOWS */

namespace lsp
{
    namespace ipc
    {
        SharedMutex::SharedMutex()
        {
            hLock       = NULL;
            bLocked     = false;
        }

        SharedMutex::~SharedMutex()
        {
            close();
        }

        status_t SharedMutex::open(const char *name)
        {
            if (hLock != NULL)
                return STATUS_OPENED;

            LSPString tmp;
        #ifdef PLATFORM_WINDOWS
            if (!tmp.set_utf8(name))
                return STATUS_NO_MEM;
        #else
            if (!tmp.append(FILE_SEPARATOR_C))
                return STATUS_NO_MEM;
            if (!tmp.append_utf8(name))
                return STATUS_NO_MEM;
        #endif /* PLATFORM_WINDOWS */
            return open_internal(&tmp);
        }

        status_t SharedMutex::open(const LSPString *name)
        {
            if (hLock != NULL)
                return STATUS_OPENED;

        #ifdef PLATFORM_WINDOWS
            return open_internal(name);
        #else
            LSPString tmp;

            if (!tmp.append(FILE_SEPARATOR_C))
                return STATUS_NO_MEM;
            if (!tmp.append(name))
                return STATUS_NO_MEM;

            return open_internal(&tmp);
        #endif /* PLATFORM_WINDOWS */
        }

        status_t SharedMutex::open_internal(const LSPString *name)
        {
        #ifdef PLATFORM_WINDOWS
            const WCHAR *path = name->get_utf16();
            if (path == NULL)
                return STATUS_NO_MEM;

            hLock = CreateMutexW(NULL, FALSE, path);
            if (hLock != NULL)
                return STATUS_OK;
        #else
            const char *path = name->get_native();
            if (name == NULL)
                return STATUS_NO_MEM;

            static constexpr int open_mode =
                S_IRUSR | S_IWUSR |
                S_IRGRP | S_IWGRP |
                S_IROTH | S_IWOTH;

            hLock = sem_open(path, O_CREAT, open_mode, 1);
            if (hLock != NULL)
                return STATUS_OK;

            int error = errno;
            switch (error)
            {
                case EACCES: return STATUS_PERMISSION_DENIED;
                case EEXIST: return STATUS_ALREADY_EXISTS;
                case EINVAL: return STATUS_INVALID_VALUE;
                case EMFILE: return STATUS_OVERFLOW;
                case ENFILE: return STATUS_OVERFLOW;
                case ENAMETOOLONG: return STATUS_TOO_BIG;
                case ENOENT: return STATUS_NOT_FOUND;
                case ENOMEM: return STATUS_NO_MEM;
                default: break;
            }
        #endif /* PLATFORM_WINDOWS */

            return STATUS_IO_ERROR;
        }

        status_t SharedMutex::close()
        {
            if (hLock == NULL)
                return STATUS_OK;

            status_t res = STATUS_OK;

        #ifdef PLATFORM_WINDOWS
            if (bLocked)
            {
                if (!ReleaseMutex(hLock))
                    res     = update_status(res, STATUS_IO_ERROR);
                bLocked = false;
            }

            if (!CloseHandle(hLock))
                res     = update_status(res, STATUS_IO_ERROR);
            hLock = NULL;
        #else
            if (bLocked)
            {
                if (sem_post(hLock) < 0)
                    res     = update_status(res, STATUS_IO_ERROR);
                bLocked = false;
            }
            if (sem_close(hLock) < 0)
                res     = update_status(res, STATUS_IO_ERROR);
            hLock = NULL;
        #endif /* PLATFORM_WINDOWS */

            return res;
        }

        status_t SharedMutex::lock()
        {
            if (hLock == NULL)
                return STATUS_CLOSED;
            if (bLocked)
                return STATUS_LOCKED;

        #ifdef PLATFORM_WINDOWS
            DWORD res = WaitForSingleObject(hLock, INFINITE);
            switch (res)
            {
                case WAIT_OBJECT_0:
                case WAIT_ABANDONED:
                    bLocked     = true;
                    return STATUS_OK;
                case WAIT_TIMEOUT:
                    return STATUS_TIMED_OUT;
                case WAIT_FAILED:
                    return STATUS_UNKNOWN_ERR;
                default:
                    break;
            }
        #else
            if (sem_wait(hLock) >= 0)
            {
                bLocked = true;
                return STATUS_OK;
            }

            int error = errno;
            switch (error)
            {
                case EINTR: return STATUS_INTERRUPTED;
                case EINVAL: return STATUS_INVALID_VALUE;
                case EAGAIN: return STATUS_RETRY;
                case ETIMEDOUT: return STATUS_TIMED_OUT;
                default: break;
            }
        #endif /* PLATFORM_WINDOWS */

            return STATUS_UNKNOWN_ERR;
        }

        status_t SharedMutex::lock(system::time_millis_t delay)
        {
            if (hLock == NULL)
                return STATUS_CLOSED;
            if (bLocked)
                return STATUS_LOCKED;

        #ifdef PLATFORM_WINDOWS
            DWORD res = WaitForSingleObject(hLock, delay);
            switch (res)
            {
                case WAIT_OBJECT_0:
                case WAIT_ABANDONED:
                    bLocked     = true;
                    return STATUS_OK;
                case WAIT_TIMEOUT:
                    return STATUS_TIMED_OUT;
                case WAIT_FAILED:
                    return STATUS_UNKNOWN_ERR;
                default:
                    break;
            }
        #else
            // sem_timedwait() is the same as sem_wait(), except that abs_timeout specifies a limit on the
            // amount of time that the call should block if the decrement cannot be immediately performed.
            // The abs_timeout argument points to a structure that specifies an absolute timeout in seconds
            // and nanoseconds since the Epoch, 1970-01-01 00:00:00 +0000 (UTC).
            struct timespec timeout;
            clock_gettime(CLOCK_REALTIME, &timeout);

            timeout.tv_sec  += delay / 1000;
            timeout.tv_nsec += (delay % 1000) * 1000000;
            if (timeout.tv_nsec >= 1000000000)
            {
                timeout.tv_sec     += 1;
                timeout.tv_nsec    -= 1000000000;
            }

            if (sem_timedwait(hLock, &timeout) >= 0)
            {
                bLocked = true;
                return STATUS_OK;
            }

            int error = errno;
            switch (error)
            {
                case EINTR: return STATUS_INTERRUPTED;
                case EINVAL: return STATUS_INVALID_VALUE;
                case EAGAIN: return STATUS_RETRY;
                case ETIMEDOUT: return STATUS_TIMED_OUT;
                default: break;
            }
        #endif /* PLATFORM_WINDOWS */

            return STATUS_UNKNOWN_ERR;
        }

        status_t SharedMutex::try_lock()
        {
            if (hLock == NULL)
                return STATUS_CLOSED;
            if (bLocked)
                return STATUS_LOCKED;

        #ifdef PLATFORM_WINDOWS
            DWORD res = WaitForSingleObject(hLock, 0);
            switch (res)
            {
                case WAIT_OBJECT_0:
                case WAIT_ABANDONED:
                    bLocked     = true;
                    return STATUS_OK;
                case WAIT_TIMEOUT:
                    return STATUS_RETRY;
                case WAIT_FAILED:
                    return STATUS_UNKNOWN_ERR;
                default:
                    break;
            }
        #else
            if (sem_trywait(hLock) >= 0)
            {
                bLocked     = true;
                return STATUS_OK;
            }

            int error = errno;
            switch (error)
            {
                case EINTR: return STATUS_INTERRUPTED;
                case EINVAL: return STATUS_INVALID_VALUE;
                case EAGAIN: return STATUS_RETRY;
                case ETIMEDOUT: return STATUS_TIMED_OUT;
                default: break;
            }
        #endif /* PLATFORM_WINDOWS */

            return STATUS_UNKNOWN_ERR;
        }

        status_t SharedMutex::unlock()
        {
            if (hLock == NULL)
                return STATUS_CLOSED;
            if (!bLocked)
                return STATUS_BAD_STATE;

        #ifdef PLATFORM_WINDOWS
            if (ReleaseMutex(hLock))
            {
                bLocked = false;
                return STATUS_OK;
            }
        #else
            if (sem_post(hLock) >= 0)
            {
                bLocked = false;
                return STATUS_OK;
            }

            int error = errno;
            switch (error)
            {
                case EINTR: return STATUS_INTERRUPTED;
                case EINVAL: return STATUS_INVALID_VALUE;
                case EAGAIN: return STATUS_RETRY;
                case ETIMEDOUT: return STATUS_TIMED_OUT;
                default: break;
            }
        #endif /* PLATFORM_WINDOWS */

            return STATUS_UNKNOWN_ERR;
        }

    } /* namespace ipc */
} /* namespace lsp */


