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
#include <lsp-plug.in/runtime/system.h>

#ifdef PLATFORM_WINDOWS
    #include <windows.h>
    #include <memoryapi.h>
#else
    #include <errno.h>
    #include <fcntl.h>
    #include <semaphore.h>
    #include <sys/mman.h>
    #include <sys/stat.h>
    #include <sys/types.h>
    #include <unistd.h>
#endif /* PLATFORM_WINDOWS */

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
        #ifdef PLATFORM_WINDOWS
            LSPString   sLock;              // Lock name
            HANDLE      hFD;                // File descriptor
            HANDLE      hMapping;           // File mapping
        #else
            int         hFD;                // File descriptor
        #endif /* PLATFORM_WINDOWS */
        };

        SharedMem::SharedMem()
        {
            pContext        = NULL;
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
            release_context();
        }

        void SharedMem::release_context()
        {
            if (pContext == NULL)
                return;
            if (atomic_add(&pContext->nReferences, -1) != 1)
                return;

            close_context(pContext);
            pContext = NULL;
            delete pContext;
        }

        SharedMem & SharedMem::operator = (const SharedMem & src)
        {
            release_context();

            pContext        = src.pContext;
            if (pContext != NULL)
                atomic_add(&pContext->nReferences, 1);

            return *this;
        }

        SharedMem & SharedMem::operator = (SharedMem && src)
        {
            release_context();

            pContext        = src.pContext;
            src.pContext    = NULL;

            return *this;
        }

        bool SharedMem::allocate_context()
        {
            if (pContext != NULL)
                return false;
            shared_context_t *ctx   = new shared_context_t;
            if (ctx == NULL)
                return false;

            ctx->nReferences    = 1;
            ctx->pData          = NULL;
            ctx->nSize          = 0;
            ctx->nMapOffset     = 0;
            ctx->nMapSize       = 0;
            ctx->nMode          = 0;

        #ifdef PLATFORM_WINDOWS
            ctx->hFD            = NULL;
            ctx->hMapping       = NULL;
        #else
            ctx->hFD            = -1;
        #endif /* PLATFORM_WINDOWS */

            pContext = ctx;

            return true;
        }

        status_t SharedMem::open(const char *name, size_t mode, size_t size)
        {
            if (name == NULL)
                return STATUS_BAD_ARGUMENTS;

            LSPString tmp;

        #ifdef PLATFORM_WINDOWS
            if (!tmp.set_utf8(name))
                return STATUS_NO_MEM;
        #else
            // For portable use, a shared memory object should be identified by a name of the form  /somename;
            // that is, a null-terminated string of up to NAME_MAX (i.e., 255) characters consisting of an initial
            // slash, followed by one or more characters, none of which are slashes.
            if (!tmp.append(FILE_SEPARATOR_C))
                return STATUS_NO_MEM;
            if (!tmp.append_utf8(name))
                return STATUS_NO_MEM;
        #endif /* PLATFORM_WINDOWS */

            if (!allocate_context())
                return STATUS_NO_MEM;

            return open_context(pContext, &tmp, mode, size);
        }

        status_t SharedMem::open(const LSPString *name, size_t mode, size_t size)
        {
            if (name == NULL)
                return STATUS_BAD_ARGUMENTS;

            LSPString tmp;
        #ifdef PLATFORM_WINDOWS
            if (!tmp.set(name))
                return STATUS_NO_MEM;
        #else
            // For portable use, a shared memory object should be identified by a name of the form  /somename;
            // that is, a null-terminated string of up to NAME_MAX (i.e., 255) characters consisting of an initial
            // slash, followed by one or more characters, none of which are slashes.
            if (!tmp.append(FILE_SEPARATOR_C))
                return STATUS_NO_MEM;
            if (!tmp.append(name))
                return STATUS_NO_MEM;
        #endif /* PLATFORM_WINDOWS */

            if (!allocate_context())
                return STATUS_NO_MEM;

            return open_context(pContext, &tmp, mode, size);
        }

        status_t SharedMem::close_context(shared_context_t *ctx)
        {
            status_t res = STATUS_OK;
            if (ctx == NULL)
                return res;

            res = update_status(res, unmap_context(ctx));
            res = update_status(res, close_file(ctx));

            return res;
        }

        status_t SharedMem::close_file(shared_context_t *ctx)
        {
            status_t res = STATUS_OK;

        #ifdef PLATFORM_WINDOWS
            lsp_finally {
                ctx->sPath.truncate();
                ctx->sLock.truncate();
            };

            const bool is_open = (ctx->hFD != NULL);
            if (is_open)
            {
                if (!CloseHandle(ctx->hFD))
                    res             = STATUS_IO_ERROR;
                ctx->hFD        = NULL;
            }
        #else
            lsp_finally {
                ctx->sPath.truncate();
            };

            // Close file handle and remove it if persistent mode was specified
            const bool is_open = (ctx->hFD >= 0);
            if (is_open)
            {
                if (::close(ctx->hFD) < 0)
                    res             = STATUS_IO_ERROR;
                ctx->hFD = -1;
            }
        #endif /* PLATFORM_WINDOWS */

            // Need to release system structures?
            if ((ctx->nMode & (SHM_CREATE | SHM_PERSIST)) == SHM_CREATE)
                res = update_status(res, unlink_file(ctx));

            return res;
        }

        status_t SharedMem::unmap_context(shared_context_t *ctx)
        {
            if (ctx->pData == NULL)
                return STATUS_OK;
            lsp_finally { ctx->pData = NULL; };

        #ifdef PLATFORM_WINDOWS
            status_t res = STATUS_OK;
            if (!UnmapViewOfFile(ctx->pData))
                res = STATUS_IO_ERROR;

            if (ctx->hMapping != NULL)
            {
                if (!CloseHandle(ctx->hMapping))
                    res = STATUS_IO_ERROR;
                ctx->hMapping   = NULL;
            }

            return res;
        #else
            if (munmap(ctx->pData, ctx->nMapSize) >= 0)
                return STATUS_OK;

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
        #endif /* PLATFORM_WINDOWS */
        }

        status_t SharedMem::unlink_file(shared_context_t *ctx)
        {
            status_t res = STATUS_OK;
        #ifdef PLATFORM_WINDOWS
            // TODO
        #else
            const char *path = ctx->sPath.get_native();
            if (path == NULL)
                return STATUS_NO_MEM;

            if (shm_unlink(path) < 0)
                res = STATUS_IO_ERROR;
        #endif /* PLATFORM_WINDOWS */

            return res;
        }

        status_t SharedMem::open_context(shared_context_t *ctx, LSPString *name, size_t mode, size_t size)
        {
            lsp_finally {
                close_context(ctx);
            };

        #ifdef PLATFORM_WINDOWS
            // TODO

        #else
            if (ctx->hFD >= 0)
                return STATUS_OPENED;

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
            ctx->nMode          = mode & (~SHM_PERSIST);
            ctx->sPath.swap(name);

            static constexpr int open_mode =
                S_IRUSR | S_IWUSR |
                S_IRGRP | S_IWGRP |
                S_IROTH | S_IWOTH;

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
            ctx->hFD            = fd;

            // Resize memory segment if it was created
            if (mode & SHM_CREATE)
            {
                if (ftruncate(ctx->hFD, size) < 0)
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

                size            = attr.size;
            }

        #endif /* PLATFORM_WINDOWS */

            // Now we are ready to deploy new context
            ctx->nMode      = mode;
            ctx->nSize      = size;

            // Prevent context from being closed
            ctx             = NULL;

            return STATUS_OK;
        }

        status_t SharedMem::close()
        {
            status_t res = close_context(pContext);
            pContext = NULL;
            return res;
        }

        status_t SharedMem::map_context(shared_context_t *ctx, size_t offset, size_t size)
        {
        #ifdef PLATFORM_WINDOWS
             void *addr = NULL;

             // TODO

         #else
             int prot_flags = 0;
             if ((ctx->nMode & SHM_READ) != 0)
                 prot_flags     |= PROT_READ;
             if ((ctx->nMode & SHM_WRITE) != 0)
                 prot_flags     |= PROT_WRITE;
             if ((ctx->nMode & SHM_EXEC) != 0)
                 prot_flags     |= PROT_EXEC;

             // Map new memory address
             void *addr = mmap(0, size, prot_flags, MAP_SHARED, ctx->hFD, offset);
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
             if (ctx->pData != NULL)
                 munmap(ctx->pData, ctx->nMapSize);

         #endif /* PLATFORM_WINDOWS */

             // Commit new state
             ctx->pData         = addr;
             ctx->nMapOffset    = offset;
             ctx->nMapSize      = size;

             return STATUS_OK;
        }

        status_t SharedMem::map(size_t offset, size_t size)
        {
            if (!opened())
                return STATUS_CLOSED;

            return map_context(pContext, offset, size);
        }

        status_t SharedMem::unmap()
        {
            // Check state
            if (!opened())
                return STATUS_CLOSED;
            if (pContext->pData == NULL)
                return STATUS_NOT_MAPPED;

            // Unmap memory
            return unmap_context(pContext);
        }

        void *SharedMem::data()
        {
            return (pContext != NULL) ? pContext->pData : NULL;
        }

        const void *SharedMem::data() const
        {
            return (pContext != NULL) ? pContext->pData : NULL;
        }

        ssize_t SharedMem::map_offset() const
        {
            if (!opened())
                return -STATUS_CLOSED;
            if (pContext->pData == NULL)
                return -STATUS_NOT_MAPPED;

            return pContext->nMapOffset;
        }

        ssize_t SharedMem::map_size() const
        {
            if (!opened())
                return -STATUS_CLOSED;
            if (pContext->pData == NULL)
                return -STATUS_NOT_MAPPED;

            return pContext->nMapSize;
        }

        wssize_t SharedMem::size() const
        {
            if (!opened())
                return -STATUS_CLOSED;

            return pContext->nSize;
        }

        bool SharedMem::opened() const
        {
            if (pContext == NULL)
                return false;
        #ifdef PLATFORM_WINDOWS
            if (pContext->hFD == NULL)
                return false;
        #else
            if (pContext->hFD < 0)
                return false;
        #endif /* PLATFORM_WINDOWS */

            return true;
        }

        bool SharedMem::mapped() const
        {
            return (pContext != NULL) ? pContext->pData != NULL : false;
        }

    } /* namespace ipc */
} /* namespace lsp */

