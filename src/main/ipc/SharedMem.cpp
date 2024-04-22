/*
 * Copyright (C) 2024 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2024 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-runtime-lib
 * Created on: 21 апр. 2024 г.
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

#include <lsp-plug.in/ipc/SharedMem.h>

#include <lsp-plug.in/runtime/version.h>

#include <lsp-plug.in/common/atomic.h>
#include <lsp-plug.in/common/status.h>
#include <lsp-plug.in/io/File.h>
#include <lsp-plug.in/runtime/LSPString.h>

#include <errno.h>
#include <fcntl.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>


namespace lsp
{
    namespace ipc
    {
        struct shared_context_t
        {
            uatomic_t   nReferences;        // Number of references to this object
            void       *pData;              // Pointer to the mapped memory buffer
            wsize_t     nSize;              // Original segment size
            size_t      nMapOffset;         // Mapping offset
            size_t      nMapSize;           // Mapping size
            size_t      nMode;              // Open mode
            LSPString   sPath;              // Path to the shared memory segment

            // System-specific data
            int         nFD;                // File descriptor
//            sem_t      *pSemaphore;         // Named semaphore
        };

        SharedMem::SharedMem()
        {
            pContext    = NULL;
        }

        SharedMem::SharedMem(const SharedMem & src)
        {
            pContext        = src.pContext;
            if (pContext != NULL)
                atomic_add(&pContext->nReferences, 1);
        }

        SharedMem::SharedMem(SharedMem && src)
        {
            pContext        = src.pContext;
            src.pContext    = NULL;
        }

        SharedMem::~SharedMem()
        {
            if (pContext == NULL)
                return;
            if (atomic_add(&pContext->nReferences, -1) != 1)
                return;

            destroy_context(pContext);
            pContext        = NULL;
        }

        SharedMem & SharedMem::operator = (const SharedMem & src)
        {
            close();

            pContext        = src.pContext;
            if (pContext != NULL)
                atomic_add(&pContext->nReferences, 1);
            return *this;
        }

        SharedMem & SharedMem::operator = (SharedMem && src)
        {
            close();

            pContext        = src.pContext;
            src.pContext    = NULL;
            if (pContext != NULL)
                atomic_add(&pContext->nReferences, 1);
            return *this;
        }

        status_t SharedMem::open(const char *name, size_t mode, size_t size)
        {
            if (name == NULL)
                return STATUS_BAD_ARGUMENTS;
            else if (pContext != NULL)
                return STATUS_OPENED;

            // For portable use, a shared memory object should be identified by a name of the form  /somename;
            // that is, a null-terminated string of up to NAME_MAX (i.e., 255) characters consisting of an initial
            // slash, followed by one or more characters, none of which are slashes.
            LSPString tmp;
            if (!tmp.append(FILE_SEPARATOR_C))
                return STATUS_NO_MEM;
            if (!tmp.append_utf8(name))
                return STATUS_NO_MEM;

            return open_internal(&tmp, mode, size);
        }

        status_t SharedMem::open(const LSPString *name, size_t mode, size_t size)
        {
            if (name == NULL)
                return STATUS_BAD_ARGUMENTS;
            else if (pContext != NULL)
                return STATUS_OPENED;

            // For portable use, a shared memory object should be identified by a name of the form  /somename;
            // that is, a null-terminated string of up to NAME_MAX (i.e., 255) characters consisting of an initial
            // slash, followed by one or more characters, none of which are slashes.
            LSPString tmp;
            if (!tmp.append(FILE_SEPARATOR_C))
                return STATUS_NO_MEM;
            if (!tmp.append(name))
                return STATUS_NO_MEM;

            return open_internal(&tmp, mode, size);
        }

        void SharedMem::destroy_context(shared_context_t *ctx)
        {
            if (ctx == NULL)
                return;

            // Unmap contents
            if (ctx->pData != NULL)
            {
                munmap(ctx->pData, ctx->nMapSize);
                ctx->pData      = NULL;
            }

            // Close file handle and remove it if persistent mode was specified
            if (ctx->nFD >= 0)
                ::close(ctx->nFD);

//            // Close semaphore if present
//            if (ctx->pSemaphore != NULL)
//                sem_close(ctx->pSemaphore);

            // Need to release system structures?
            if ((ctx->nMode & (SHM_CREATE | SHM_PERSIST)) == SHM_CREATE)
            {
                const char *path = ctx->sPath.get_native();
                if (path != NULL)
                {
                    if (ctx->nFD >= 0)
                        shm_unlink(path);
//                    if (ctx->pSemaphore != NULL)
//                        sem_unlink(path);
                }
            }

            ctx->nFD = -1;
//            ctx->pSemaphore = NULL;

            delete ctx;
        }

        status_t SharedMem::open_internal(LSPString *name, size_t mode, size_t size)
        {
            // Form the open mode
            int o_flags = 0;
            if ((mode & (SHM_READ | SHM_WRITE)) == 0)
                return STATUS_BAD_ARGUMENTS;

            if ((mode & SHM_WRITE) != 0)
                o_flags     = O_RDWR;
            else
                o_flags     = O_RDONLY;
            if (mode & SHM_CREATE)
                o_flags    |= O_CREAT | O_EXCL;

            // Get the path
            const char *path = name->get_native();
            if (path == NULL)
                return STATUS_NO_MEM;

            // Create and initialize context
            shared_context_t *ctx = new shared_context_t();
            if (ctx == NULL)
                return STATUS_NO_MEM;

            ctx->nReferences    = 1;
            ctx->pData          = NULL;
            ctx->nSize          = 0;
            ctx->nMapOffset     = 0;
            ctx->nMapSize       = 0;
            ctx->nMode          = mode & (~SHM_PERSIST);
            ctx->sPath.swap(name);

            ctx->nFD            = -1;
//            ctx->pSemaphore     = NULL;

            lsp_finally {
                destroy_context(ctx);
            };

            static constexpr int open_mode =
                S_IRUSR | S_IWUSR |
                S_IRGRP | S_IWGRP |
                S_IROTH | S_IWOTH;

//            // Open/create semaphore for locks
//            ctx->pSemaphore = sem_open(path, o_flags & (O_CREAT | O_EXCL), open_mode, 1);
//            if (ctx->pSemaphore == NULL)
//            {
//                const int error = errno;
//                switch (error)
//                {
//                    case EACCES: return STATUS_PERMISSION_DENIED;
//                    case EEXIST: return STATUS_ALREADY_EXISTS;
//                    case EINVAL: return STATUS_INVALID_VALUE;
//                    case EMFILE: return STATUS_OVERFLOW;
//                    case ENAMETOOLONG: return STATUS_OVERFLOW;
//                    case ENFILE: return STATUS_OVERFLOW;
//                    case ENOENT: return STATUS_NOT_FOUND;
//                    case ENOMEM: return STATUS_NO_MEM;
//                    default: break;
//                }
//                return STATUS_IO_ERROR;
//            }

            // Open/create shared memory segment
            const int fd = shm_open(path, o_flags, open_mode);
            if (fd < 0)
            {
                const int error = errno;
                switch (error)
                {
                    case EACCES: return STATUS_PERMISSION_DENIED;
                    case EEXIST: return STATUS_ALREADY_EXISTS;
                    case EINVAL: return STATUS_INVALID_VALUE;
                    case EMFILE: return STATUS_OVERFLOW;
                    case ENAMETOOLONG: return STATUS_OVERFLOW;
                    case ENFILE: return STATUS_OVERFLOW;
                    case ENOENT: return STATUS_NOT_FOUND;
                    default: break;
                }
                return STATUS_IO_ERROR;
            }
            ctx->nFD            = fd;

            // Resize memory segment if it was created
            if (mode & SHM_CREATE)
            {
                if (ftruncate(ctx->nFD, size) < 0)
                {
                    const int error = errno;
                    switch (error)
                    {
                        case EACCES: return STATUS_PERMISSION_DENIED;
                        case EPERM: return STATUS_PERMISSION_DENIED;
                        case EFBIG: return STATUS_TOO_BIG;
                        default: break;
                    }
                    return STATUS_IO_ERROR;
                }
            }
            else
            {
                io::fattr_t attr;
                status_t res = io::File::stat(fd, &attr);
                if (res != STATUS_OK)
                    return res;

                ctx->nSize      = attr.size;
            }

            // Now we are ready to deploy new context
            ctx->nMode      = mode;
            lsp::swap(pContext, ctx);

            return STATUS_OK;
        }

        status_t SharedMem::close()
        {
            status_t res = STATUS_OK;

            if (pContext != NULL)
            {
                if (pContext->pData != NULL)
                    res     = update_status(res, unmap());

                if (pContext->nFD >= 0)
                    res = update_status(res, ::close(pContext->nFD));

                if ((pContext->nMode & (SHM_CREATE | SHM_PERSIST)) == SHM_CREATE)
                {
                    const char *path = pContext->sPath.get_native();
                    if (path != NULL)
                        shm_unlink(path);
                }

                delete pContext;
                pContext        = NULL;
            }

            return res;
        }

        status_t SharedMem::map(size_t offset, size_t size)
        {
            if (pContext == NULL)
                return STATUS_CLOSED;
            if (pContext->nFD < 0)
                return STATUS_CORRUPTED;

            int prot_flags = 0;
            if ((pContext->nMode & SHM_READ) != 0)
                prot_flags     |= PROT_READ;
            if ((pContext->nMode & SHM_WRITE) != 0)
                prot_flags     |= PROT_WRITE;
            if ((pContext->nMode & SHM_EXEC) != 0)
                prot_flags     |= PROT_EXEC;

            // Map new memory address
            void *addr = mmap(0, size, prot_flags, MAP_SHARED, pContext->nFD, offset);
            if (addr == MAP_FAILED)
            {
                const int error = errno;
                switch (error)
                {
                    case EACCES: return STATUS_PERMISSION_DENIED;
                    case EAGAIN: return STATUS_RETRY;
                    case EPERM: return STATUS_PERMISSION_DENIED;
                    case EFBIG: return STATUS_TOO_BIG;
                    case EEXIST: return STATUS_ALREADY_EXISTS;
                    case ENOMEM: return STATUS_NO_MEM;
                    case EOVERFLOW: return STATUS_OVERFLOW;
                    default: break;
                }
                return STATUS_IO_ERROR;
            }

            // Unmap previously mapped address
            if (pContext->pData != NULL)
                munmap(pContext->pData, pContext->nMapSize);

            // Commit new state
            pContext->pData     = addr;
            pContext->nMapOffset= offset;
            pContext->nMapSize  = size;

            return STATUS_OK;
        }

        status_t SharedMem::unmap()
        {
            // Check state
            if (pContext == NULL)
                return STATUS_CLOSED;
            if (pContext->pData == NULL)
                return STATUS_NOT_MAPPED;

            // Unmap memory
            if (munmap(pContext->pData, pContext->nMapSize) < 0)
            {
                const int error = errno;
                switch (error)
                {
                    case EACCES: return STATUS_PERMISSION_DENIED;
                    case EAGAIN: return STATUS_RETRY;
                    case EPERM: return STATUS_PERMISSION_DENIED;
                    case EFBIG: return STATUS_TOO_BIG;
                    case EEXIST: return STATUS_ALREADY_EXISTS;
                    case ENOMEM: return STATUS_NO_MEM;
                    case EOVERFLOW: return STATUS_OVERFLOW;
                    default: break;
                }
                return STATUS_IO_ERROR;
            }

            return STATUS_OK;
        }

        void *SharedMem::data()
        {
            return (pContext != NULL) ? pContext->pData : NULL;
        }

        const void *SharedMem::data() const
        {
            return (pContext != NULL) ? pContext->pData : NULL;
        }

//        status_t SharedMem::lock() const
//        {
//            if (pContext == NULL)
//                return STATUS_CLOSED;
//            if (pContext->pSemaphore == NULL)
//                return STATUS_BAD_STATE;
//
//            // Try to lock semaphore
//            int error = 0;
//            do
//            {
//                if (sem_wait(pContext->pSemaphore) == 0)
//                    return STATUS_OK;
//
//                error = errno;
//            } while (error == EAGAIN);
//
//            // Analyze error
//            switch (error)
//            {
//                case EINTR: return STATUS_INTERRUPTED;
//                case EINVAL: return STATUS_BAD_ARGUMENTS;
//                default: break;
//            }
//            return STATUS_IO_ERROR;
//        }
//
//        status_t SharedMem::try_lock() const
//        {
//            if (pContext == NULL)
//                return STATUS_CLOSED;
//            if (pContext->pSemaphore == NULL)
//                return STATUS_BAD_STATE;
//
//            // Try to lock semaphore
//            if (sem_trywait(pContext->pSemaphore) == 0)
//                return STATUS_OK;
//
//            const int error = errno;
//            if (error == EAGAIN)
//                return STATUS_RETRY;
//
//            // Analyze error
//            switch (error)
//            {
//                case EINTR: return STATUS_INTERRUPTED;
//                case EINVAL: return STATUS_BAD_ARGUMENTS;
//                default: break;
//            }
//            return STATUS_IO_ERROR;
//        }
//
//        status_t SharedMem::unlock() const
//        {
//            if (pContext == NULL)
//                return STATUS_CLOSED;
//            if (pContext->pSemaphore == NULL)
//                return STATUS_BAD_STATE;
//
//            // Unlock the semaphore
//            if (sem_post(pContext->pSemaphore) == 0)
//                return STATUS_OK;
//
//            // Analyze error
//            const int error = errno;
//            switch (error)
//            {
//                case EINTR: return STATUS_INTERRUPTED;
//                case EINVAL: return STATUS_BAD_ARGUMENTS;
//                default: break;
//            }
//            return STATUS_IO_ERROR;
//        }

        ssize_t SharedMem::map_offset() const
        {
            if (pContext == NULL)
                return -STATUS_CLOSED;
            else if (pContext->pData == NULL)
                return -STATUS_NOT_MAPPED;

            return pContext->nMapOffset;
        }

        ssize_t SharedMem::map_size() const
        {
            if (pContext == NULL)
                return -STATUS_CLOSED;
            else if (pContext->pData == NULL)
                return -STATUS_NOT_MAPPED;

            return pContext->nMapSize;
        }

        wssize_t SharedMem::size() const
        {
            if (pContext == NULL)
                return -STATUS_CLOSED;

            return pContext->nSize;
        }

        bool SharedMem::opened() const
        {
            return pContext != NULL;
        }

        bool SharedMem::mapped() const
        {
            return (pContext != NULL) ? pContext->pData != NULL : false;
        }

    } /* namespace ipc */
} /* namespace lsp */

