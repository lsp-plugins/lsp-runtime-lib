/*
 * Copyright (C) 2021 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2021 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-runtime-lib
 * Created on: 3 мар. 2021 г.
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

#ifndef LSP_PLUG_IN_IO_OUTBITSTREAM_H_
#define LSP_PLUG_IN_IO_OUTBITSTREAM_H_

#include <lsp-plug.in/runtime/version.h>
#include <lsp-plug.in/runtime/LSPString.h>
#include <lsp-plug.in/io/IInStream.h>
#include <lsp-plug.in/io/Path.h>
#include <lsp-plug.in/io/File.h>

namespace lsp
{
    namespace io
    {
        /**
         * Output bit stream
         */
        class OutBitstream
        {
            private:
                OutBitstream & operator = (const OutBitstream &);

            private:
                uint8_t        *pBuffer;    // Fixed-size buffer
                size_t          nBits;      // Number of bits stored

            public:
                explicit OutBitstream();
                ~OutBitstream();

            public:
                status_t        open(const char *path);
                status_t        open(const LSPString *path);
                status_t        open(const io::Path *path);

                status_t        wrap(FILE *fd, bool close);
                status_t        wrap_native(fhandle_t fd, bool close);
                status_t        wrap(File *fd, size_t flags);
                status_t        wrap(IOutStream *os, size_t flags);

                status_t        flush(bool full);
                status_t        close();

            public:
                status_t        writev(const void *buf, size_t bits);

                status_t        write(bool value);
                status_t        write(uint8_t value, size_t bits = sizeof(uint8_t));
                status_t        write(int8_t value, size_t bits = sizeof(int8_t));
                status_t        write(uint16_t value, size_t bits = sizeof(uint16_t));
                status_t        write(int16_t value, size_t bits = sizeof(int16_t));
                status_t        write(uint32_t value, size_t bits = sizeof(uint32_t));
                status_t        write(int32_t value, size_t bits = sizeof(int32_t));
                status_t        write(uint64_t value, size_t bits = sizeof(uint64_t));
                status_t        write(int64_t value, size_t bits = sizeof(int64_t));
        };
    }
}

#endif /* LSP_PLUG_IN_IO_OUTBITSTREAM_H_ */
