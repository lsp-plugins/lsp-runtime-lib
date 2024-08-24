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
    #include <pthread.h>
    #include <sys/file.h>
    #include <sys/mman.h>
    #include <sys/stat.h>
    #include <sys/types.h>
    #include <unistd.h>
#endif /* PLATFORM_WINDOWS */

namespace lsp
{
    namespace ipc
    {
        static constexpr uint64_t SHMUTEX_INIT_FLAG     = __IF_LEBE(0x786574756D5F6873ULL, 0x73685F6D75746578ULL); // "sh_mutex"

        typedef struct shared_mutex_t
        {
            uint64_t            init_flag;
            pthread_mutex_t     mutex;
        } shared_mutex_t;

        SharedMutex::SharedMutex()
        {
        #ifdef PLATFORM_WINDOWS
            hLock       = NULL;
        #else
            hFD         = -1;
            hLock       = NULL;
        #endif /* PLATFORM_WINDOWS */
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
            return (hLock != NULL) ? STATUS_OK : STATUS_IO_ERROR;
        #else
            int error;
            const char *path = name->get_native();
            if (name == NULL)
                return STATUS_NO_MEM;

            static constexpr int open_mode =
                S_IRUSR | S_IWUSR |
                S_IRGRP | S_IWGRP |
                S_IROTH | S_IWOTH;

            // Create shared memory segment
            int fd = shm_open(path, O_RDWR | O_CREAT, open_mode);
            if (fd < 0)
            {
                error = errno;
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
                    default: return STATUS_IO_ERROR;
                }
            }
            lsp_finally {
                if (fd >= 0)
                    ::close(fd);
            };

            // Perform mutex initialization
            shared_mutex_t *shmutex = NULL;
            {
                // Lock the shared memory segment to initialize mutex atomically
                while (flock(fd, LOCK_EX) != 0)
                {
                    error = errno;
                    switch (error)
                    {
                        case EBADF: return STATUS_IO_ERROR;
                        case EINTR: break;
                        case EINVAL: return STATUS_INVALID_VALUE;
                        case ENOLCK: return STATUS_NO_MEM;
                        default: return STATUS_IO_ERROR;
                    }
                }

                // Unlock file descriptor on exit out of the scope
                bool need_unlock = true;
                lsp_finally {
                    if (need_unlock)
                        flock(fd, LOCK_UN);
                };

                // Reserve memory for the mutex
                if (ftruncate(fd, sizeof(shared_mutex_t)) != 0)
                {
                    error = errno;
                    switch (error)
                    {
                        case EACCES: return STATUS_PERMISSION_DENIED;
                        case EFAULT: return STATUS_UNKNOWN_ERR;
                        case EFBIG: return STATUS_TOO_BIG;
                        case EINVAL: return STATUS_INVALID_VALUE;
                        case EINTR: return STATUS_INTERRUPTED;
                        case EIO: return STATUS_IO_ERROR;
                        case EISDIR: return STATUS_IS_DIRECTORY;
                        case EPERM: return STATUS_PERMISSION_DENIED;
                        case EROFS: return STATUS_READONLY;
                        case ETXTBSY: return STATUS_PERMISSION_DENIED;
                        case EBADF: return STATUS_IO_ERROR;
                        default: return STATUS_IO_ERROR;
                    }
                }

                // Map the mutex memory
                void *addr = mmap(0, sizeof(shared_mutex_t), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
                if (addr == MAP_FAILED)
                {
                    error = errno;
                    switch (error)
                    {
                        case EACCES: return STATUS_PERMISSION_DENIED;
                        case EAGAIN: return STATUS_RETRY;
                        case EPERM: return STATUS_PERMISSION_DENIED;
                        case EFBIG: return STATUS_TOO_BIG;
                        case EEXIST: return STATUS_ALREADY_EXISTS;
                        case ENOMEM: return STATUS_NO_MEM;
                        case EOVERFLOW: return STATUS_OVERFLOW;
                        default: return STATUS_IO_ERROR;
                    }
                }
                lsp_finally {
                    if (addr != NULL)
                        munmap(addr, sizeof(shared_mutex_t));
                };

                // Ensure that mutex is initialized
                shmutex     = static_cast<shared_mutex_t *>(addr);
                if (shmutex->init_flag != SHMUTEX_INIT_FLAG)
                {
                    // Mutex is not initialized, initialize it
                    pthread_mutexattr_t attr;
                    if ((error = pthread_mutexattr_init(&attr)) != 0)
                        return STATUS_UNKNOWN_ERR;

                    if ((error = pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED) != 0))
                    {
                        switch (error)
                        {
                            case EINVAL: return STATUS_INVALID_VALUE;
                            case ENOTSUP: return STATUS_NOT_IMPLEMENTED;
                            default: return STATUS_UNKNOWN_ERR;
                        }
                    }
                    if ((error = pthread_mutexattr_setrobust(&attr, PTHREAD_MUTEX_ROBUST)) != 0)
                        return STATUS_UNKNOWN_ERR;
                    if ((error = pthread_mutex_init(&shmutex->mutex, &attr)) != 0)
                        return STATUS_UNKNOWN_ERR;

                    // Mark the mutex being initialized
                    shmutex->init_flag  = SHMUTEX_INIT_FLAG;
                }

                // Unlock the file
                while (flock(fd, LOCK_UN) != 0)
                {
                    error = errno;
                    switch (error)
                    {
                        case EBADF: return STATUS_IO_ERROR;
                        case EINTR: break;
                        case EINVAL: return STATUS_INVALID_VALUE;
                        case ENOLCK: return STATUS_NO_MEM;
                        default: return STATUS_IO_ERROR;
                    }
                }

                // Do not unlock the file again
                addr = NULL;
                need_unlock = false;
            }

            // Now we have shared mutex and file descriptor, store them
            hFD     = fd;
            hLock   = shmutex;

            fd      = -1;

            return STATUS_OK;
        #endif /* PLATFORM_WINDOWS */
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
                int error = pthread_mutex_unlock(&hLock->mutex);
                if (error != 0)
                    res     = update_status(res, STATUS_IO_ERROR);
                bLocked = false;
            }

            // Unmap memory
            munmap(hLock, sizeof(shared_mutex_t));
            hLock = NULL;

            // Close file descriptor
            if (hFD >= 0)
            {
                ::close(hFD);
                hFD     = -1;
            }
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

            return STATUS_UNKNOWN_ERR;
        #else
            int error = pthread_mutex_lock(&hLock->mutex);
            if (error != 0)
            {
                switch (error)
                {
                    case EDEADLK: return STATUS_BAD_STATE;
                    case EBUSY: return STATUS_LOCKED;
                    case EOWNERDEAD:
                    {
                        pthread_mutex_consistent(&hLock->mutex);
                        break;
                    }
                    default: return STATUS_UNKNOWN_ERR;
                }
            }

            bLocked     = true;

            return STATUS_OK;
        #endif /* PLATFORM_WINDOWS */
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

            return STATUS_UNKNOWN_ERR;
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

            int error = pthread_mutex_timedlock(&hLock->mutex, &timeout);
            if (error != 0)
            {
                switch (error)
                {
                    case EDEADLK: return STATUS_BAD_STATE;
                    case EBUSY: return STATUS_LOCKED;
                    case ETIMEDOUT: return STATUS_TIMED_OUT;
                    case EOWNERDEAD:
                    {
                        pthread_mutex_consistent(&hLock->mutex);
                        break;
                    }
                    default: return STATUS_UNKNOWN_ERR;
                }
            }

            bLocked     = true;

            return STATUS_OK;
        #endif /* PLATFORM_WINDOWS */
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

            return STATUS_UNKNOWN_ERR;
        #else
            int error = pthread_mutex_trylock(&hLock->mutex);
            if (error != 0)
            {
                switch (error)
                {
                    case EDEADLK: return STATUS_BAD_STATE;
                    case EBUSY: return STATUS_RETRY;
                    case EOWNERDEAD:
                    {
                        pthread_mutex_consistent(&hLock->mutex);
                        break;
                    }
                    default: return STATUS_UNKNOWN_ERR;
                }
            }

            bLocked     = true;

            return STATUS_OK;
        #endif /* PLATFORM_WINDOWS */
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

            return STATUS_UNKNOWN_ERR;
        #else
            pthread_mutex_unlock(&hLock->mutex);
            bLocked     = false;

            return STATUS_OK;
        #endif /* PLATFORM_WINDOWS */
        }

    } /* namespace ipc */
} /* namespace lsp */


