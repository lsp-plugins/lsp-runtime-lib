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

#include <lsp-plug.in/common/alloc.h>
#include <lsp-plug.in/stdlib/string.h>
#include <lsp-plug.in/resource/buffer.h>

namespace lsp
{
    namespace resource
    {
        buffer_t::buffer_t()
        {
            data        = NULL;
            head        = 0;
            tail        = 0;
            cap         = 0;
        }

        buffer_t::~buffer_t()
        {
            destroy();
        }

        status_t buffer_t::init(size_t capacity)
        {
            uint8_t *ptr    = static_cast<uint8_t *>(realloc(data, capacity * 2));
            if (ptr == NULL)
                return STATUS_NO_MEM;

            data            = ptr;
            head            = 0;
            tail            = 0;
            cap             = capacity;

            return STATUS_OK;
        }

        void buffer_t::destroy()
        {
            if (data != NULL)
                free(data);
            data            = NULL;
            head            = 0;
            tail            = 0;
            cap             = 0;
        }

        void buffer_t::append(const void *src, ssize_t count)
        {
            const uint8_t *v    = reinterpret_cast<const uint8_t *>(src);
            ssize_t avail       = ((cap << 1) - tail);
            if (count < avail)
            {
                memcpy(&data[tail], v, count);
                tail      += count;
                head       = lsp_max(head, tail - cap);
            }
            else if (count < cap)
            {
                ssize_t head    = tail + count - cap;
                memmove(data, &data[head], tail - head);
                memcpy(&data[tail - head], v, count);
            }
            else
            {
                memcpy(data, &v[count - cap], cap);
                head       = 0;
                tail       = cap;
            }
        }

        void buffer_t::append(uint8_t v)
        {
            // Shift buffer if needed
            if (tail >= (cap << 1))
            {
                memmove(data, &data[cap], cap);
                head  -= cap;
                tail  -= cap;
            }

            // Append byte
            data[tail++]  = v;
            head          = lsp_max(head, tail - cap);
        }

        size_t buffer_t::lookup(ssize_t *out, const void *src, size_t avail)
        {
            ssize_t offset      = -1;
            size_t len          = 0;

            const uint8_t *s    = reinterpret_cast<const uint8_t *>(src);
            const uint8_t *p    = &data[head];

            for (size_t i=0, n=tail - head; i<n; ++i)
            {
                // Find first character match
                if (p[i] != *s)
                    continue;

                size_t slen     = 1; // Sequence length
                size_t count    = lsp_min(avail, n - i);
                for (size_t j=1; j<count; ++j, ++slen)
                    if (p[i+j] != s[j])
                        break;

                if (len < slen)
                {
                    offset          = i;
                    len             = slen;
                }
            }

            *out        = offset;
            return len;
        }

        void buffer_t::clear()
        {
            head            = 0;
            tail            = 0;
        }
    }
}


