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
#include <lsp-plug.in/io/OutMemoryStream.h>
#include <lsp-plug.in/io/IOutSequence.h>
#include <lsp-plug.in/io/IOutStream.h>
#include <lsp-plug.in/io/OutBitStream.h>
#include <lsp-plug.in/lltl/darray.h>
#include <lsp-plug.in/lltl/parray.h>
#include <lsp-plug.in/resource/ILoader.h>
#include <lsp-plug.in/resource/buffer.h>

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
                    ssize_t     off;        // Offset in the buffer
                    node_t     *next;       // Next node in list
                    node_t     *parent;     // Parent node
                    node_t     *child;      // Child element
                } node_t;

            protected:
                lltl::darray<raw_resource_t>    vEntries;
                lltl::parray<node_t>            vNodes;         // List of nodes
                node_t                          sRoot;          // Root node
                io::OutMemoryStream             sData;          // Data buffer
                io::OutMemoryStream             sCommands;
                buffer_t                        sBuffer;        // Buffer for caching

            protected:
                status_t            alloc_entry(raw_resource_t **r, io::Path *path, resource_type_t type);
                status_t            write_entry(raw_resource_t *r, io::IInStream *is);
                node_t             *alloc_node(uint8_t code);
                static node_t      *get_child(node_t *parent, uint8_t code);
                node_t             *add_child(node_t *parent, uint8_t code);
                status_t            update_dictionary(const void *buf, size_t bytes);
                status_t            update_dictionary2(const void *buf, size_t bytes);
                bool                add_string(const uint8_t *s, size_t len);
                status_t            dump_dict(io::IOutSequence *os, node_t *curr, LSPString *word);
                status_t            emit_buffer_command(size_t bpos, const location_t *loc);
                ssize_t             lookup_word(const uint8_t *buf, size_t len);

                static size_t       calc_repeats(const uint8_t *head, const uint8_t *tail);

                static ssize_t      lookup_dict(location_t *out, node_t *root, const uint8_t *s, size_t avail);
                static ssize_t      commit_dict(io::OutMemoryStream *out, location_t *loc, node_t *root, const uint8_t *s);

                static status_t     emit_uint(io::OutBitStream *obs, size_t value, size_t initial, size_t stepping);

            public:
                explicit Compressor();
                ~Compressor();

            public:
                /**
                 * Close the compressor
                 * @return status of operation
                 */
                status_t                close();

            public:
                inline const void      *data() const            { return sData.data();                                      }
                inline const void      *commands() const        { return sCommands.data();                                  }

                inline const raw_resource_t *entries() const    { return vEntries.array();                                  }

                inline size_t           data_size() const       { return sData.size();                                      }
                inline size_t           commands_size() const   { return sCommands.size();                                  }
                inline size_t           num_entires() const     { return vEntries.size();                                   }

                /**
                 * Start a new segment of data. May be useful for sorting data by different types
                 * @return status of operation
                 */
                status_t                flush();

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

                /**
                 * Perform the compression
                 * @return compression
                 */
                status_t                compress();

                /**
                 * Dump contents to stream
                 * @param os output stream
                 */
                status_t                dump_dictionary(io::IOutStream *os);
        };
    }
}



#endif /* LSP_PLUG_IN_RESOURCE_ICOMPRESSOR_H_ */
