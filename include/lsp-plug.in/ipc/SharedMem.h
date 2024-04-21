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

#ifndef LSP_PLUG_IN_IPC_SHAREDMEM_H_
#define LSP_PLUG_IN_IPC_SHAREDMEM_H_

#include <lsp-plug.in/runtime/version.h>

#include <lsp-plug.in/common/status.h>
#include <lsp-plug.in/runtime/LSPString.h>

namespace lsp
{
    namespace ipc
    {
        struct shared_context_t;

        /**
         * Named shared memory segment.
         */
        class SharedMem
        {
            public:
                enum mode_t
                {
                    SHM_READ        = 1 << 0,   // Read access
                    SHM_WRITE       = 1 << 1,   // Write access
                    SHM_EXEC        = 1 << 2,   // Execute access
                    SHM_CREATE      = 1 << 3,   // Create segment if it does not exist
                    SHM_PERSIST     = 1 << 4,   // Persistent flag: do not remove associated file

                    SHM_RW          = SHM_READ | SHM_WRITE,
                    SHM_RWX         = SHM_READ | SHM_WRITE | SHM_EXEC
                };

            private:
                mutable shared_context_t *pContext;

            protected:
                status_t open_internal(LSPString *name, size_t mode, size_t size);
                static void destroy_context(shared_context_t *ctx);

            public:
                SharedMem();
                SharedMem(const SharedMem & src);
                SharedMem(SharedMem && src);
                ~SharedMem();
                SharedMem & operator = (const SharedMem & src);
                SharedMem & operator = (SharedMem && src);

            public:
                /**
                 * Create named shared memory segment and map to memory
                 * @param name shared memory segment identifier (UTF-8)
                 * @param mode memory access mode
                 * @param size the size of shared memory segment if it should be created
                 * @param offset the offset relative to the beginning of the shared memory segment
                 * @return status of operation
                 */
                status_t open(const char *name, size_t mode, size_t size);

                /**
                 * Create named shared memory segment and map to memory
                 * @param name shared memory segment identifier
                 * @param mode memory access mode
                 * @param size the size of shared memory segment if it should be created
                 * @param offset the offset relative to the beginning of the shared memory segment
                 * @return status of operation
                 */
                status_t open(const LSPString *name, size_t mode, size_t size);

                /**
                 * Close named shared memory segment
                 * @return status of operation
                 */
                status_t close();

                /**
                 * Map shared segment to the specified offset. If the segment already was mapped,
                 * the previous mapping is released on success.
                 * @param offset the offset from the beginning of the segment
                 * @param size the number of bytes to map
                 * @return status of operation
                 */
                status_t map(size_t offset, size_t size);

                /**
                 * Unmap current memory mapping
                 * @return status of operation
                 */
                status_t unmap();

            public:
                /**
                 * Check if shared memory segment is opened
                 * @return true if shared memory segment is opened
                 */
                bool opened() const;

                /**
                 * Check if shared memory segment is opened
                 * @return true if shared memory segment is opened
                 */
                bool mapped() const;

                /**
                 * Return the pointer to the mapped memory
                 * @return pointer to the mapped memory or NULL if shared memory segment is not mapped
                 */
                void *data();

                /**
                 * Return the pointer to the mapped memory
                 * @return pointer to the mapped memory or NULL if shared memory segment is not mapped
                 */
                const void *data() const;

//                /**
//                 * Lock the shared memory segment. Blocking operations are available when shared segment
//                 * is created and do not depend on it's mapping.
//                 * @return status of operation
//                 */
//                status_t lock() const;
//
//                /**
//                 * Try to lock the shared memory segment. Blocking operations are available when shared segment
//                 * is created and do not depend on it's mapping.
//                 * @return status of operation: STATUS_OK if locked, STATUS_RETRY if not locked
//                 */
//                status_t try_lock() const;
//
//                /**
//                 * Unlock the shared memory segment. Blocking operations are available when shared segment
//                 * is created and do not depend on it's mapping.
//                 * @return status of operation
//                 */
//                status_t unlock() const;

                /**
                 * Return mapping offset
                 * @return mapping offset or negative error code
                 */
                ssize_t map_offset() const;

                /**
                 * Return mapping size
                 * @return mapping size or negative error code
                 */
                ssize_t map_size() const;

                /**
                 * Return size of the whole shared memory segment
                 * @return the size of the whole shared memory segment
                 */
                wssize_t size() const;
        };

    } /* namespace ipc */
} /* namespace lsp */


#endif /* LSP_PLUG_IN_IPC_SHAREDMEM_H_ */