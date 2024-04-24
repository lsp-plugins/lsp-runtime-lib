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

#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <time.h>

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
            if (!tmp.set_utf8(name))
                return STATUS_NO_MEM;
            return open(&tmp);
        }

        status_t SharedMutex::open(const LSPString *name)
        {
            if (hLock != NULL)
                return STATUS_OPENED;

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
            return STATUS_IO_ERROR;
        }

        status_t SharedMutex::close()
        {
            if (hLock == NULL)
                return STATUS_OK;

            status_t res = STATUS_OK;
            if (bLocked)
            {
                if (sem_post(hLock) < 0)
                    res     = update_status(res, STATUS_IO_ERROR);
                bLocked = false;
            }
            if (sem_close(hLock) < 0)
                res     = update_status(res, STATUS_IO_ERROR);
            hLock = NULL;

            return res;
        }

        status_t SharedMutex::lock()
        {
            if (hLock == NULL)
                return STATUS_CLOSED;
            if (bLocked)
                return STATUS_LOCKED;

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

            return STATUS_UNKNOWN_ERR;
        }

        status_t SharedMutex::lock(system::time_millis_t delay)
        {
            if (hLock == NULL)
                return STATUS_CLOSED;
            if (bLocked)
                return STATUS_LOCKED;

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

            return STATUS_UNKNOWN_ERR;
        }

        status_t SharedMutex::try_lock()
        {
            if (hLock == NULL)
                return STATUS_CLOSED;
            if (bLocked)
                return STATUS_LOCKED;

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

            return STATUS_UNKNOWN_ERR;
        }

        status_t SharedMutex::unlock()
        {
            if (hLock == NULL)
                return STATUS_CLOSED;
            if (!bLocked)
                return STATUS_BAD_STATE;

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

            return STATUS_UNKNOWN_ERR;
        }

    } /* namespace ipc */
} /* namespace lsp */


