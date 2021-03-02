/*
 * Copyright (C) 2021 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2021 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-runtime-lib
 * Created on: 1 мар. 2021 г.
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

#ifndef LSP_PLUG_IN_RESOURCE_ICOMPRESSOR_H_
#define LSP_PLUG_IN_RESOURCE_ICOMPRESSOR_H_

#include <lsp-plug.in/runtime/version.h>
#include <lsp-plug.in/resource/ILoader.h>
#include <lsp-plug.in/io/OutMemoryStream.h>
#include <lsp-plug.in/lltl/darray.h>

namespace lsp
{
    namespace resource
    {
        /**
         * Interface for resource compressor
         */
        class Compressor
        {
            private:
                Compressor & operator = (const Compressor &);

            protected:
                lltl::darray<raw_resource_t>    vEntries;
                io::OutMemoryStream             sBuffer;
                io::OutMemoryStream             sCommands;

            protected:
                status_t            alloc_entry(raw_resource_t *r, io::Path *path, resource_type_t type);
                status_t            write_entry(raw_resource_t *r, io::IInStream *is);

            protected:
                explicit Compressor();
                ~Compressor();

            public:
                /**
                 * Close the compressor
                 * @return status of operation
                 */
                status_t                close();

            public:
                template <class T>
                    inline const T         *data() const        { return reinterpret_cast<const T *>(sBuffer.data());       }
                template <class T>
                    inline const T         *commands() const    { return reinterpret_cast<const T *>(sCommands.data());     }
                inline const raw_resource_t *entries() const    { return vEntries.array();                                  }

                inline size_t           data_size() const       { return sBuffer.size();                                    }
                inline size_t           commands_size() const   { return sCommands.size();                                  }
                inline size_t           num_entires() const     { return vEntries.size();                                   }

                /**
                 * Create resource entry and start it's writing
                 * @param name resource entry name
                 * @param is input stream to read entry from
                 * @return status of operation
                 */
                status_t                create_file(const char *name, io::IInStream *is);
                status_t                create_file(const LSPString *name, io::IInStream *is);
                status_t                create_file(const io::Path *name, io::IInStream *is);

                /**
                 * Create directory
                 * @param name
                 * @return
                 */
                status_t                create_dir(const char *name);
                status_t                create_dir(const LSPString *name);
                status_t                create_dir(const io::Path *name);
        };
    }
}



#endif /* LSP_PLUG_IN_RESOURCE_ICOMPRESSOR_H_ */
