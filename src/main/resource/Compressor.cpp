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

#include <lsp-plug.in/resource/Compressor.h>
#include <lsp-plug.in/stdlib/string.h>

namespace lsp
{
    namespace resource
    {
        Compressor::Compressor()
        {
            sRoot.code      = 0;
            sRoot.hits      = 0;
            sRoot.next      = NULL;
            sRoot.child     = NULL;
        }

        Compressor::~Compressor()
        {
            close();
        }

        status_t Compressor::close()
        {
            // Drop nodes
            for (size_t i=0, n=vNodes.size(); i<n; ++i)
            {
                node_t *node    = vNodes.get(i);
                if (node != NULL)
                    free(node);
            }
            vNodes.flush();

            sRoot.code      = 0;
            sRoot.hits      = 0;
            sRoot.child     = NULL;
            sRoot.next      = NULL;

            // Drop entries
            for (size_t i=0, n=vEntries.size(); i < n; ++i)
            {
                raw_resource_t *res = vEntries.uget(i);
                if (res == NULL)
                    continue;
                if (res->name != NULL)
                    free(const_cast<char *>(res->name));
                res->name   = NULL;
            }
            vEntries.flush();

            // Drop buffer data
            sBuffer.drop();
            sCommands.drop();

            status_t res    = sBuffer.close();
            status_t res2   = sCommands.close();
            if (res == STATUS_OK)
                res         = res2;

            return res;
        }

        Compressor::node_t *Compressor::add_child(node_t *parent, uint8_t code)
        {
            node_t *node    = static_cast<node_t *>(malloc(sizeof(node_t)));
            if (node == NULL)
                return NULL;
            if (!vNodes.add(node))
            {
                free(node);
                return NULL;
            }

            // Initialize node and link to parent
            node->code      = code;
            node->hits      = 0;
            node->next      = parent->child;
            node->child     = NULL;
            parent->child   = node;

            return node;
        }

        Compressor::node_t *Compressor::get_child(node_t *parent, uint8_t code)
        {
            for (node_t *child = parent->child; child != NULL; child = child->next)
                if (child->code == code)
                {
                    ++child->hits;
                    return child;
                }
            return NULL;
        }

        status_t Compressor::alloc_entry(raw_resource_t *r, io::Path *path, resource_type_t type)
        {
            if (path->is_empty())
                return STATUS_BAD_ARGUMENTS;

            ssize_t         index   = -1;
            LSPString       item;
            status_t        res;

            while (true)
            {
                // Get first item and remove it
                if ((res = path->pop_first(&item)) != STATUS_OK)
                    return res;

                // Lookup for existing directory/create yet another one
                raw_resource_t *found       = NULL;
                for (size_t i=0, n=vEntries.size(); i<n; ++i)
                {
                    raw_resource_t *ent         = vEntries.uget(i);
                    if ((ent == NULL) || (ent->parent != index) || (ent->name == NULL))
                        continue;
                    if (item.equals_utf8(ent->name))
                    {
                        found                   = ent;
                        break;
                    }
                }

                // Last entry?
                if (path->is_empty())
                {
                    // Item already exists?
                    if (found != NULL)
                        return STATUS_ALREADY_EXISTS;

                    // Add new resource entry
                    found   = vEntries.add();
                    if (found == NULL)
                        return STATUS_NO_MEM;

                    // Initialize
                    found->type     = type;
                    found->parent   = index;
                    found->offset   = 0;
                    found->length   = 0;
                    found->name     = item.clone_utf8();

                    if (found->name == NULL)
                        return STATUS_NO_MEM;

                    return STATUS_OK;
                }
                else
                {
                    // Need to create new one?
                    if (found == NULL)
                    {
                        // Allocate item
                        if ((found = vEntries.add()) == NULL)
                            return STATUS_NO_MEM;

                        found->type     = RES_DIR;
                        found->parent   = index;
                        found->offset   = 0;
                        found->length   = 0;
                        found->name     = item.clone_utf8();

                        if (found->name == NULL)
                            return STATUS_NO_MEM;
                    }

                    // Update position
                    index       = vEntries.index_of(found);
                }
            }
        }

        bool Compressor::add_string(const uint8_t *s, size_t len)
        {
            node_t *curr            = &sRoot;

            for (size_t i=0; i<len; ++i)
            {
                // Lookup for existing child node
                node_t *next        = get_child(curr, s[i]);
                if (next != NULL)
                {
                    curr                = next;
                    continue;
                }

                // Allocate yet another child node
                curr                = add_child(curr, s[i]);
                if (curr == NULL)
                    return false;
            }

            // All is OK, return
            return true;
        }

        status_t Compressor::update_dictionary(const void *buf, size_t bytes)
        {
            const uint8_t *ptr      = static_cast<const uint8_t *>(buf);
            const uint8_t *end      = &ptr[bytes];
            node_t *curr            = &sRoot;

            while (ptr < end)
            {
                uint8_t b           = *(ptr++);
                node_t *next        = get_child(curr, b);

                // Word was found?
                if (next != NULL)
                {
                    curr                = next; // Move to child node
                    continue;
                }

                // No word found at all?
                if (curr == &sRoot)
                {
                    // Move pointer until we get at least one duplicated character
                    const uint8_t *head     = ptr - 1;
                    while (ptr < end)
                    {
                        if (memchr(head, *ptr, ptr - head) != NULL)
                            break;
                        b                       = *(ptr++);
                    }

                    // Now index all strings from head to ptr
                    // For string "12345"
                    // Register strings "12345", "2345", "345", "45" and "5"
                    for (; head < ptr; ++head)
                    {
                        if (!add_string(head, ptr - head))
                            return STATUS_NO_MEM;
                    }
                }
                else
                {
                    // Word was not found, need to add
                    if (!add_child(curr, b))
                        return STATUS_NO_MEM;

                    // Reset current pointer to root
                    curr    = &sRoot;
                }

                // Skip repeated characters
                while ((ptr < end) && (*ptr == b))
                    ++ptr;
            }

            return STATUS_OK;
        }

        status_t Compressor::write_entry(raw_resource_t *r, io::IInStream *is)
        {
            // Copy all data to memory
            io::OutMemoryStream os;
            wssize_t length     = is->sink(&os);
            status_t res        = (length >= 0) ? STATUS_OK : status_t(-length);

            // Build dictionary
            if (res == STATUS_OK)
                res         = update_dictionary(os.data(), os.size());
            if (res == STATUS_OK)
            {
                length      = sBuffer.write(os.data(), os.size());
                res         = (length >= 0) ? STATUS_OK : status_t(-length);
            }

            // Drop data
            os.drop();
            os.close();
            return res;
        }

        status_t Compressor::create_file(const char *name, io::IInStream *is)
        {
            io::Path tmp;
            raw_resource_t r;

            status_t res = tmp.set(name);
            if (res == STATUS_OK)
                res     = alloc_entry(&r, &tmp, RES_FILE);
            if (res == STATUS_OK)
                res     = write_entry(&r, is);

            return res;
        }

        status_t Compressor::create_file(const LSPString *name, io::IInStream *is)
        {
            io::Path tmp;
            raw_resource_t r;

            status_t res = tmp.set(name);
            if (res == STATUS_OK)
                res     = alloc_entry(&r, &tmp, RES_FILE);
            if (res == STATUS_OK)
                res     = write_entry(&r, is);

            return res;
        }

        status_t Compressor::create_file(const io::Path *name, io::IInStream *is)
        {
            io::Path tmp;
            raw_resource_t r;

            status_t res = tmp.set(name);
            if (res == STATUS_OK)
                res     = alloc_entry(&r, &tmp, RES_FILE);
            if (res == STATUS_OK)
                res     = write_entry(&r, is);

            return res;
        }

        status_t Compressor::create_dir(const char *name)
        {
            io::Path tmp;
            raw_resource_t r;

            status_t res = tmp.set(name);
            if (res == STATUS_OK)
                res     = alloc_entry(&r, &tmp, RES_DIR);

            return res;
        }

        status_t Compressor::create_dir(const LSPString *name)
        {
            io::Path tmp;
            raw_resource_t r;

            status_t res = tmp.set(name);
            if (res == STATUS_OK)
                res     = alloc_entry(&r, &tmp, RES_DIR);

            return res;
        }

        status_t Compressor::create_dir(const io::Path *name)
        {
            io::Path tmp;
            raw_resource_t r;

            status_t res = tmp.set(name);
            if (res == STATUS_OK)
                res     = alloc_entry(&r, &tmp, RES_DIR);

            return res;
        }
    }
}


