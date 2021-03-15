/*
 * Copyright (C) 2021 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2021 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-runtime-lib
 * Created on: 16 мар. 2021 г.
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

#include <lsp-plug.in/io/InMemoryStream.h>
#include <lsp-plug.in/resource/Decompressor.h>

namespace lsp
{
    namespace resource
    {
        Decompressor::Decompressor()
        {
            nOffset     = 0;
            nFirst      = 0;
            nLast       = 0;
            nRep        = 0;
            nByte       = 0;
        }

        Decompressor::~Decompressor()
        {
        }

        status_t Decompressor::init(const void *data, size_t first, size_t length, size_t buf_sz)
        {
            // Create buffer
            status_t res = sBuffer.init(buf_sz);
            if (res != STATUS_OK)
                return res;

            // Create inpute memory stream
            io::InMemoryStream *ims = new io::InMemoryStream(data, SIZE_MAX);
            if (ims == NULL)
                return STATUS_NO_MEM;

            // Wrap input memory stream
            res = sIn.wrap(ims, WRAP_CLOSE | WRAP_DELETE);
            if (res != STATUS_OK)
            {
                ims->close();
                delete ims;
            }

            // Update positions
            nOffset     = 0;
            nFirst      = first;
            nLast       = first + length;
            nRep        = 0;
            nByte       = 0;

            return res;
        }

        status_t Decompressor::read_uint(size_t *out, size_t initial, size_t stepping)
        {
            ssize_t res;
            ssize_t bits    = initial;
            size_t value    = 0;
            bool flag;

            while (true)
            {
                if ((res = sIn.readb(&flag)) != 1)
                    return (res < 0) ? -res : STATUS_IO_ERROR;
                if (!flag)
                    break;

                value      += 1 << bits;
                bits       += stepping;
            }

            size_t v        = 0;
            if ((res = sIn.readv(&v, bits)) != bits)
                return (res < 0) ? -res : STATUS_IO_ERROR;

            *out            = value + v;
            return STATUS_OK;
        }

        ssize_t Decompressor::read_internal()
        {
            // TODO: implement this

//            uint8_t *d      = reinterpret_cast<uint8_t *>(dst);
//            size_t nread    = 0;
//            status_t res    = STATUS_OK;
//            size_t offset, length, rep;
//
//            while (nread < count)
//            {
//                // Check offset
//                if (nOffset >= nLast)
//                {
//                    if (nread > 0)
//                        break;
//                    return -set_error(STATUS_EOF);
//                }
//
//                // Repeat character?
//                if (nRep > 0)
//                {
//                    if (nOffset >= nFirst)
//                        d[nread++]  = nByte;
//                    ++nOffset;
//                    --nRep;
//                    continue;
//                }
//
//                // Read offset
//                if ((res = read_uint(&offset, 5, 5)) != STATUS_OK)
//                {
//                    if (nread > 0)
//                        break;
//                    return -set_error(res);
//                }
//
//                // Check type of event
//                if (offset >= sBuffer.size())
//                {
//                    // OCTET
//                    // Value
//                    nByte       = offset - sBuffer.size();
//
//                    // Repeat
//                    if ((res = read_uint(&rep, 0, 4)) != STATUS_OK)
//                    {
//                        if (nread > 0)
//                            break;
//                        return -set_error(res);
//                    }
//
//                    // Append buffer
//                    length      = 1 + lsp_max(rep, 4);
//                    for (size_t i=0; i<length; ++i)
//                    {
//                        if ((res = sBuffer.append(nByte)) != STATUS_OK)
//                            return -set_error(res);
//                    }
//
//                }
//                else
//                {
//                    //
//                }
//            }
//
//            return nread;
            return -1;
        }

        ssize_t Decompressor::read(void *dst, size_t count)
        {
            ssize_t res;
            uint8_t *d      = reinterpret_cast<uint8_t *>(dst);
            size_t nread    = 0;

            while (nread < count)
            {
                // Check offset
                if (nOffset >= nLast)
                    break;

                // Read byte
                if ((res = read_internal()) < 0)
                {
                    if (nread > 0)
                        break;
                    set_error(-res);
                    return res;
                }

                // Check if we can emit result
                if ((nOffset++) >= nFirst)
                    d[nread++]      = res;
            }

            return (nread > 0) ? nread : -set_error(STATUS_EOF);
        }

        ssize_t Decompressor::read_byte()
        {
            ssize_t res;

            if (nOffset >= nLast)
                return -set_error(STATUS_EOF);

            while (true)
            {
                // Read byte
                if ((res = read_internal()) < 0)
                    break;

                // Check if we can emit result
                if ((nOffset++) >= nFirst)
                    return res;
            }

            set_error(-res);
            return res;
        }

        status_t Decompressor::close()
        {
            sBuffer.destroy();
            return sIn.close();
        }
    }
}
