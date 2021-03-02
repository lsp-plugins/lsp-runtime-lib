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
#include <lsp-plug.in/lltl/parray.h>

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
                typedef struct node_t
                {
                    uint8_t     code;       // The octet code
                    wsize_t     hits;       // Number of hits for the node
                    node_t     *next;       // Next node in list
                    node_t     *child;      // Child element
                } node_t;

            protected:
                lltl::darray<raw_resource_t>    vEntries;
                lltl::parray<node_t>            vNodes;         // List of nodes
                node_t                          sRoot;          // Root node
                io::OutMemoryStream             sBuffer;        // Data buffer
                io::OutMemoryStream             sCommands;

            protected:
                status_t            alloc_entry(raw_resource_t *r, io::Path *path, resource_type_t type);
                status_t            write_entry(raw_resource_t *r, io::IInStream *is);
                node_t             *alloc_node(uint8_t code);
                static node_t      *get_child(node_t *parent, uint8_t code);
                node_t             *add_child(node_t *parent, uint8_t code);
                status_t            update_dictionary(const void *buf, size_t bytes);
                bool                add_strings(const uint8_t *s, size_t len);

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
