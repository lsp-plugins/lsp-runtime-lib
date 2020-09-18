/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-runtime-lib
 * Created on: 13 мар. 2019 г.
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

#ifndef LSP_PLUG_IN_IO_OUTFILESTREAM_H_
#define LSP_PLUG_IN_IO_OUTFILESTREAM_H_

#include <lsp-plug.in/runtime/version.h>
#include <lsp-plug.in/runtime/LSPString.h>
#include <lsp-plug.in/io/Path.h>
#include <lsp-plug.in/io/File.h>
#include <lsp-plug.in/io/IOutStream.h>
#include <lsp-plug.in/stdlib/stdio.h>

namespace lsp
{
    namespace io
    {
        class OutFileStream: public IOutStream
        {
            private:
                File       *pFD;
                size_t      nWrapFlags;

            private:
                OutFileStream & operator = (const OutFileStream &);

            public:
                explicit OutFileStream();
                virtual ~OutFileStream();

            public:
                status_t wrap(FILE *fd, bool close);

                status_t wrap_native(fhandle_t fd, bool close);

                status_t wrap(File *fd, size_t flags);

                status_t open(const char *path, size_t mode);

                status_t open(const LSPString *path, size_t mode);

                status_t open(const Path *path, size_t mode);

                virtual wssize_t    position();

                virtual ssize_t     write(const void *buf, size_t count);

                virtual wssize_t    seek(wsize_t position);

                virtual status_t    flush();

                virtual status_t    close();
        };
    
    } /* namespace io */
} /* namespace lsp */

#endif /* LSP_PLUG_IN_IO_OUTFILESTREAM_H_ */
