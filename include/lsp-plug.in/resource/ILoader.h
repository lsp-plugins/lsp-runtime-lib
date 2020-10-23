/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-runtime-lib
 * Created on: 23 окт. 2020 г.
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

#ifndef LSP_PLUG_IN_IO_ILOADER_H_
#define LSP_PLUG_IN_IO_ILOADER_H_

#include <lsp-plug.in/runtime/version.h>
#include <lsp-plug.in/io/IInSequence.h>
#include <lsp-plug.in/io/IInStream.h>
#include <lsp-plug.in/io/Path.h>

namespace lsp
{
    namespace resource
    {
        /**
         * Interface that allows to resolve the resource name and open input stream/sequence
         * By default works like just a factory to the input file stream and input file sequence
         */
        class ILoader
        {
            private:
                ILoader & operator      = (const ILoader &);

            protected:
                status_t            nError;

            public:
                explicit ILoader();
                virtual ~ILoader();

            public:
                inline status_t             last_error() const      { return nError; }

                virtual io::IInStream      *read_stream(const char *name);
                virtual io::IInStream      *read_stream(const LSPString *name);

                /**
                 * This is the main method to be overloaded
                 * @param name name of resource
                 * @return pointer to opened stream or NULL and error code is set
                 */
                virtual io::IInStream      *read_stream(const io::Path *name);

                virtual io::IInSequence    *read_sequence(const char *name, const char *charset = NULL);
                virtual io::IInSequence    *read_sequence(const LSPString *name, const char *charset = NULL);
                virtual io::IInSequence    *read_sequence(const io::Path *name, const char *charset = NULL);
        };
    }
}


#endif /* LSP_PLUG_IN_IO_ILOADER_H_ */
