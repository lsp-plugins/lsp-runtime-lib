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
        cbuffer_t::cbuffer_t()
        {
            data        = NULL;
            index       = NULL;
            head        = 0;
            tail        = 0;
            cap         = 0;
        }

        cbuffer_t::~cbuffer_t()
        {
            destroy();
        }

        status_t cbuffer_t::init(size_t capacity)
        {
            size_t dbuf     = capacity * 2 * sizeof(uint8_t);
            size_t ibuf     = capacity * 2 * sizeof(uint32_t);

            uint8_t *ptr    = static_cast<uint8_t *>(realloc(data, dbuf + ibuf));
            if (ptr == NULL)
                return STATUS_NO_MEM;

            data            = ptr;
            index           = reinterpret_cast<uint32_t *>(&ptr[dbuf]);
            head            = 0;
            tail            = 0;
            cap             = capacity;

            return STATUS_OK;
        }

        void cbuffer_t::destroy()
        {
            if (data != NULL)
                free(data);
            data            = NULL;
            index           = NULL;
            head            = 0;
            tail            = 0;
            cap             = 0;
        }

        void cbuffer_t::append(const void *src, ssize_t count)
        {
            const uint8_t *v    = reinterpret_cast<const uint8_t *>(src);
            ssize_t avail       = ((cap << 1) - tail);
            if (count < avail)
            {
                memcpy(&data[tail], v, count * sizeof(uint8_t));
                bzero(&index[tail], count * sizeof(uint32_t));
                tail      += count;
                head       = lsp_max(head, tail - cap);
            }
            else if (count < cap)
            {
                ssize_t head    = tail + count - cap;
                memmove(data, &data[head], tail - head);
                memcpy(&data[tail - head], v, count);

                memmove(index, &index[head], (tail - head) * sizeof(uint32_t));
                bzero(&index[tail - head], count * sizeof(uint32_t));
            }
            else
            {
                memcpy(data, &v[count - cap], cap * sizeof(uint8_t));
                bzero(index, cap * sizeof(uint32_t));

                head       = 0;
                tail       = cap;
            }
        }

        void cbuffer_t::append(uint8_t v)
        {
            // Shift buffer if needed
            if (tail >= (cap << 1))
            {
                memmove(data, &data[cap],   cap * sizeof(uint8_t));
                memmove(index, &index[cap], cap * sizeof(uint32_t));
                head  -= cap;
                tail  -= cap;
            }

            // Append byte
            data[tail]      = v;        // Data byte
            index[tail]     = 0;        // No index
            ++tail;
            head            = lsp_max(head, tail - cap);
        }

        size_t cbuffer_t::lookup(ssize_t *out, const void *src, size_t avail)
        {
            ssize_t offset      = -1;
            size_t len          = 0;

            const uint8_t *s    = reinterpret_cast<const uint8_t *>(src);
            const uint8_t *p    = &data[head];
            uint32_t *ix        = &index[head];
            ssize_t ixp         = -1;
            size_t di           = 0;

            for (size_t i=0, n=tail - head; i<(n - len); i += di)
            {
                // Find first character match
                if (p[i] != *s)
                {
                    di          = 1;
                    continue;
                }

                // Symbol matched, update index it if needed
                if ((ixp >= 0) && (!ix[ixp]))
                    ix[ixp]     = uint32_t(i - ixp);
                ixp = i;                            // Save index as previous match
                di  = (ix[i]) ? ix[i] : 1;

                // Quick test
                if (len >= 3)
                {
                    // Check last byte
                    size_t last   = len - 1;
                    if (p[i + last] != s[last])
                        continue;

                    // Check middle byte
                    last >>= 1;
                    if (p[i + last] != s[last])
                        continue;
                }

                // Perform full test
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

        void cbuffer_t::clear()
        {
            head            = 0;
            tail            = 0;
        }

        dbuffer_t::dbuffer_t()
        {
            data        = NULL;
            head        = 0;
            tail        = 0;
            cap         = 0;
        }

        dbuffer_t::~dbuffer_t()
        {
            destroy();
        }

        status_t dbuffer_t::init(size_t capacity)
        {
            size_t dbuf     = capacity * 2 * sizeof(uint8_t);
            uint8_t *ptr    = static_cast<uint8_t *>(realloc(data, dbuf));
            if (ptr == NULL)
                return STATUS_NO_MEM;

            data            = ptr;
            head            = 0;
            tail            = 0;
            cap             = capacity;

            return STATUS_OK;
        }

        void dbuffer_t::destroy()
        {
            if (data != NULL)
                free(data);
            data            = NULL;
            head            = 0;
            tail            = 0;
            cap             = 0;
        }

        void dbuffer_t::append(const void *src, ssize_t count)
        {
            const uint8_t *v    = reinterpret_cast<const uint8_t *>(src);
            ssize_t avail       = ((cap << 1) - tail);
            if (count < avail)
            {
                memcpy(&data[tail], v, count * sizeof(uint8_t));
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
                memcpy(data, &v[count - cap], cap * sizeof(uint8_t));
                head       = 0;
                tail       = cap;
            }
        }

        void dbuffer_t::append(uint8_t v)
        {
            // Shift buffer if needed
            if (tail >= (cap << 1))
            {
                memmove(data, &data[cap], cap * sizeof(uint8_t));
                head  -= cap;
                tail  -= cap;
            }

            // Append byte
            data[tail]      = v;        // Data byte
            ++tail;
            head            = lsp_max(head, tail - cap);
        }

        void dbuffer_t::clear()
        {
            head            = 0;
            tail            = 0;
        }
    }
}


