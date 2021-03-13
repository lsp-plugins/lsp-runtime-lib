/*
 * Copyright (C) 2021 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2021 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-runtime-lib
 * Created on: 13 мар. 2021 г.
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

#ifndef LSP_PLUG_IN_RESOURCE_BUFFER_H_
#define LSP_PLUG_IN_RESOURCE_BUFFER_H_

#include <lsp-plug.in/runtime/version.h>
#include <lsp-plug.in/common/types.h>
#include <lsp-plug.in/common/status.h>

namespace lsp
{
    namespace resource
    {
        /**
         * Location in the buffer
         */
        typedef struct location_t
        {
            ssize_t     offset;     // Offset relative to the buffer
            size_t      len;        // Length of the block
            size_t      repeat;     // Number of repeats
        } location_t;

        /**
         * Compression buffer
         */
        typedef struct buffer_t
        {
            public:
                uint8_t    *data;       // Buffer data (2 x capacity)
                ssize_t     head;       // Head of the buffer
                ssize_t     tail;       // Buffer tail
                ssize_t     cap;        // Buffer capacity

            public:
                explicit buffer_t();
                ~buffer_t();

                status_t        init(size_t capacity);
                void            destroy();

            public:
                void            append(const void *src, ssize_t count);
                void            append(uint8_t v);
                size_t          lookup(ssize_t *out, const void *src, size_t avail);
                void            clear();
                inline size_t   size() const { return tail - head; }

        } buffer_t;
    }
}



#endif /* LSP_PLUG_IN_RESOURCE_BUFFER_H_ */
