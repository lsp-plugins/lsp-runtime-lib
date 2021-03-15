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

#ifndef LSP_PLUG_IN_RESOURCE_DECOMPRESSOR_H_
#define LSP_PLUG_IN_RESOURCE_DECOMPRESSOR_H_

#include <lsp-plug.in/runtime/version.h>
#include <lsp-plug.in/io/IInStream.h>
#include <lsp-plug.in/io/InBitStream.h>
#include <lsp-plug.in/resource/buffer.h>

namespace lsp
{
    namespace resource
    {
        class Decompressor: public io::IInStream
        {
            private:
                Decompressor &operator = (const Decompressor &);

            protected:
                io::InBitStream                 sIn;            // Input stream
                dbuffer_t                       sBuffer;        // Buffer for caching
                size_t                          nOffset;        // Offset
                size_t                          nFirst;         // First byte
                size_t                          nLast;          // Last byte
                size_t                          nRep;           // Number of repeats
                uint8_t                         nByte;          // Byte to repeat

            protected:
                status_t                        read_uint(size_t *out, size_t initial, size_t stepping);
                ssize_t                         read_internal();

            public:
                explicit Decompressor();
                virtual ~Decompressor();

            public:
                status_t            init(const void *data, size_t first, size_t length, size_t buf_sz);

            public:
                virtual ssize_t     read_byte();
                virtual ssize_t     read(void *dst, size_t count);
                virtual status_t    close();
        };
    }
}

#endif /* LSP_PLUG_IN_RESOURCE_DECOMPRESSOR_H_ */
