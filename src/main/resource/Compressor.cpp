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
#include <lsp-plug.in/stdlib/stdio.h>
#include <lsp-plug.in/common/debug.h>
#include <lsp-plug.in/io/OutSequence.h>

namespace lsp
{
    namespace resource
    {
        Compressor::Compressor()
        {
            sRoot.code      = 0;
            sRoot.hits      = 0;
            sRoot.next      = NULL;
            sRoot.parent    = NULL;
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
            sRoot.next      = NULL;
            sRoot.parent    = NULL;
            sRoot.child     = NULL;

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
            sData.drop();
            sBuffer.drop();
            sCommands.drop();

            status_t res    = sData.close();
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
            node->parent    = parent;
            node->child     = NULL;
            parent->child   = node;

            return node;
        }

        Compressor::node_t *Compressor::get_child(node_t *parent, uint8_t code)
        {
            for (node_t *child = parent->child; child != NULL; child = child->next)
                if (child->code == code)
                    return child;
            return NULL;
        }

        status_t Compressor::alloc_entry(raw_resource_t **r, io::Path *path, resource_type_t type)
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
                    found->data     = NULL;

                    if (found->name == NULL)
                        return STATUS_NO_MEM;

                    // Return the pointer
                    if (r != NULL)
                        *r              = found;
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
                        found->data     = NULL;

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
                    curr                = next; // Move to child node and repeat
                    continue;
                }

                // No word found at all?
                if (curr == &sRoot)
                {
                    // Word was not found, need to add
                    if (!(next = add_child(curr, b)))
                        return STATUS_NO_MEM;
                    ++ next->hits;
                }
                else
                {
                    // Word was not found, need to add
                    if (!add_child(curr, b))
                        return STATUS_NO_MEM;

                    // Increment number of hits for each node
                    for ( ; curr != &sRoot; curr = curr->parent)
                        ++ curr->hits;
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
                r->offset   = sData.size();
                r->length   = length;
                length      = sData.write(os.data(), os.size());
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
            raw_resource_t *r = NULL;

            status_t res = tmp.set(name);
            if (res == STATUS_OK)
                res     = alloc_entry(&r, &tmp, RES_FILE);
            if (res == STATUS_OK)
                res     = write_entry(r, is);

            return res;
        }

        status_t Compressor::create_file(const LSPString *name, io::IInStream *is)
        {
            io::Path tmp;
            raw_resource_t *r = NULL;

            status_t res = tmp.set(name);
            if (res == STATUS_OK)
                res     = alloc_entry(&r, &tmp, RES_FILE);
            if (res == STATUS_OK)
                res     = write_entry(r, is);

            return res;
        }

        status_t Compressor::create_file(const io::Path *name, io::IInStream *is)
        {
            io::Path tmp;
            raw_resource_t *r = NULL;

            status_t res = tmp.set(name);
            if (res == STATUS_OK)
                res     = alloc_entry(&r, &tmp, RES_FILE);
            if (res == STATUS_OK)
                res     = write_entry(r, is);

            return res;
        }

        status_t Compressor::create_dir(const char *name)
        {
            io::Path tmp;

            status_t res = tmp.set(name);
            if (res == STATUS_OK)
                res     = alloc_entry(NULL, &tmp, RES_DIR);

            return res;
        }

        status_t Compressor::create_dir(const LSPString *name)
        {
            io::Path tmp;

            status_t res = tmp.set(name);
            if (res == STATUS_OK)
                res     = alloc_entry(NULL, &tmp, RES_DIR);

            return res;
        }

        status_t Compressor::create_dir(const io::Path *name)
        {
            io::Path tmp;

            status_t res = tmp.set(name);
            if (res == STATUS_OK)
                res     = alloc_entry(NULL, &tmp, RES_DIR);

            return res;
        }

        status_t Compressor::dump_dictionary(io::IOutStream *os)
        {
            LSPString word;
            io::OutSequence out;
            status_t res;

            if ((res = out.wrap(os, WRAP_NONE, "UTF-8")) != STATUS_OK)
                return res;

            res = dump_dict(&out, &sRoot, &word);
            status_t xres = out.close();

            return (res == STATUS_OK) ? xres : res;
        }

        status_t Compressor::dump_dict(io::IOutSequence *os, node_t *curr, LSPString *word)
        {
            LSPString tmp;
            status_t res;

            size_t len = word->length();
            for (node_t *child = curr->child; child != NULL; child = child->next)
            {
                word->append(child->code);

                if (tmp.fmt_ascii("\n[%5d]: ", int(child->hits)) <= 0)
                    return STATUS_NO_MEM;
                if ((res = os->write(&tmp)) != STATUS_OK)
                    return res;
                if ((res = os->write(word)) != STATUS_OK)
                    return res;
                if ((res = dump_dict(os, child, word)) != STATUS_OK)
                    return res;

                word->set_length(len);
            }

            return STATUS_OK;
        }

        ssize_t Compressor::lookup_word(const uint8_t *buf, size_t len)
        {
            node_t *curr = &sRoot;

            for (size_t n=0; n<len; ++n)
            {
                uint8_t b           = *(buf++);
                node_t *next        = get_child(curr, b);
                if (next == NULL)
                    return n;

                curr                = next;
            }

            return len;
        }

        status_t Compressor::emit_to_buffer(const uint8_t *s, size_t len)
        {
            const uint8_t *buf  = sBuffer.data();
            const uint8_t *last = &buf[sBuffer.size()];
            size_t rest         = len;

            for ( ; buf < last; ++buf)
            {
                // Check first match
                if (*buf != *s)
                    continue;

                // Check full match
                size_t compare  = lsp_min(size_t(last - buf), len);
                if (memcmp(buf, s, compare) != 0)
                    continue;

                // Buffers matched
                rest     = len - compare;
                break;
            }

//            IF_TRACE(LSPString tmp);

            if (rest == 0)
            {
//                IF_TRACE(
//                    tmp.set_ascii(reinterpret_cast<const char *>(s), len);
//                    lsp_trace("hits %s\n", tmp.get_native());
//                );
                return STATUS_OK;
            }

            // Append buffer with rest data
//            IF_TRACE(
//                tmp.set_ascii(reinterpret_cast<const char *>(&s[len-rest]), rest);
//                lsp_trace("miss %s\n", tmp.get_native());
//            );

            ssize_t res = sBuffer.write(&s[len-rest], rest);
            if (res < ssize_t(rest))
                return (res < 0) ? -res : STATUS_NO_MEM;

            return STATUS_OK;
        }

        ssize_t Compressor::lookup_buffer(location_t *out, const uint8_t *s, size_t len)
        {
            out->offset     = -1;
            out->len        = 0;
            out->repeat     = 0;

            const uint8_t *p    = sBuffer.data();
            for (size_t i=0, n=sBuffer.size(); i<n; ++i)
            {
                // Find first character match
                if (p[i] != *s)
                    continue;

                size_t slen     = 1; // Sequence length
                size_t count    = lsp_min(len, n - i);
                for (size_t j=1; j<count; ++j, ++slen)
                    if (p[i+j] != s[j])
                        break;

                // Compute the number of repetitions
                size_t rpt      = 0;
                for (size_t j = slen; j < len; ++j, ++rpt)
                    if (s[slen-1] != s[j])
                        break;

                if ((out->len + out->repeat) < (slen + rpt))
                {
                    out->offset     = i;
                    out->len        = slen;
                    out->repeat     = rpt;
                }
            }

            return out->len + out->repeat;
        }

        status_t Compressor::emit_buffer_command(size_t bpos, const location_t *loc)
        {
            uint8_t buf[64];
            ssize_t blen = 0;

            // Compute the relative offset
            ssize_t delta       = loc->offset;//loc->offset - bpos;

            // Emit data offset
            size_t i            = (delta >= 0) ? (delta << 1) : ((-delta) << 1) | 1;
            i                   = (i << 2) | ((loc->repeat >= 0x3) ? 0x3 : loc->repeat);
            do
            {
                buf[blen++]     = (i < 0x80) ? i : 0x80 | (i & 0x7f);
                i             >>= 7;
            } while (i > 0);

            // Emit data length and flag
            i                   = loc->len;
            do
            {
                buf[blen++]     = (i < 0x80) ? i : 0x80 | (i & 0x7f);
                i             >>= 7;
            } while (i > 0);

            // Additional repeat flag
            if (loc->repeat >= 0x3)
            {
                i       = loc->repeat;
                do
                {
                    buf[blen++]     = (i < 0x80) ? i : 0x80 | (i & 0x7f);
                    i             >>= 7;
                } while (i > 0);
            }

            // Now write to buffer
            ssize_t written = sCommands.write(buf, blen);
            if (written < 0)
                return -written;
            return (written == blen) ? STATUS_OK : STATUS_NO_MEM;
        }

        status_t Compressor::compress()
        {
            status_t res;
            location_t loc;
            IF_TRACE(
                LSPString tmp;
            )

            // Pre-build buffer
            for (size_t i=0, n=vEntries.size(); i<n; ++i)
            {
                raw_resource_t *r = vEntries.uget(i);
                if ((r == NULL) || (r->type != RES_FILE))
                    continue;

                lsp_trace("  preprocessing entry: %s", r->name);
                const uint8_t *head = sData.data();
                head               += r->offset;
                const uint8_t *tail = &head[r->length];
//                uint8_t b;

                while (head < tail)
                {
                    // Lookup for the dictionary word
                    ssize_t len = lookup_word(head, tail-head);
                    if (len < 0)
                        return -len;
                    else if (len == 0)
                        len = 1;

                    // Emit word to buffer
                    if ((res = emit_to_buffer(head, len)) != STATUS_OK)
                        return res;

                    // Update pointer
                    head       += len;

                    // Skip repeted characters
                    uint8_t b   = head[-1];
                    while ((head < tail) && (b == *head))
                        ++head;
                }
            }

            // Build commands
            for (size_t i=0, n=vEntries.size(); i<n; ++i)
            {
                raw_resource_t *r = vEntries.uget(i);
                if ((r == NULL) || (r->type != RES_FILE))
                    continue;

                lsp_trace("  processing entry: %s", r->name);
                const uint8_t *head = sData.data();
                head               += r->offset;
                const uint8_t *tail = &head[r->length];
                r->offset           = sCommands.size();
                size_t buf_pos      = 0;

                while (head < tail)
                {
                    // Lookup for the dictionary word
                    ssize_t len = lookup_buffer(&loc, head, tail-head);
                    if (len > 0)
                    {
                        status_t res = emit_buffer_command(buf_pos, &loc);
                        if (res != STATUS_OK)
                            return res;

                        // Update pointer
                        head       += len;
                        buf_pos     = loc.offset + len;
                    }
                    else
                    {
                        // TODO
                        ++head;
                    }
                }

                sCommands.flush();
            }

            //                lsp_trace("  compressing entry: %s", r->name);
            //                const uint8_t *head = sData.data();
            //                head               += r->offset;
            //                const uint8_t *tail = &head[r->length];
            //
            //                while (head < tail)
            //                {
            //                    status_t res = lookup_buffer(&loc, head, tail - head);
            //                    if (res != STATUS_OK)
            //                        return res;
            //
            //                    #ifdef LSP_TRACE
            //                        LSPString tmp;
            //                        const uint8_t *buf = sBuffer.data();
            //                        tmp.set_ascii(reinterpret_cast<const char *>(&buf[loc.offset]), loc.len);
            //                        uint8_t ch = buf[loc.offset + loc.len - 1];
            //                        for (size_t i=0; i<loc.repeat; ++i)
            //                            tmp.append(ch);
            //
            //                        lsp_trace("  off=%d, len=%d, rep=%d, out=%s", );
            //                    #endif /* LSP_TRACE */
            //                }

            return STATUS_OK;
        }
    }
}


