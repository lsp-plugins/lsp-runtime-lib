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
#include <lsp-plug.in/io/OutBitStream.h>
#include <lsp-plug.in/common/bits.h>
#include <lsp-plug.in/common/alloc.h>

#define BUFFER_CAPACITY         0x100000

namespace lsp
{
    enum compress_cmd_t
    {
        COMMAND_APPEND_7B,
        COMMAND_APPEND_8B,

        COMMAND_TOTAL,
        COMMAND_MAX     = COMMAND_TOTAL - 1
    };

    namespace resource
    {
        Compressor::Compressor()
        {
            sRoot.code      = 0;
            sRoot.hits      = 0;
            sRoot.off       = -1;
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
            sBuffer.destroy();
            vNodes.flush();

            sRoot.code      = 0;
            sRoot.hits      = 0;
            sRoot.off       = -1;
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
            node->off       = -1;
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
                    found->cbytes   = 0;
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
                        found->cbytes   = 0;
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
                    if (!(curr = add_child(curr, b)))
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

        status_t Compressor::update_dictionary2(const void *buf, size_t bytes)
        {
            location_t locd;
            const uint8_t *ptr      = static_cast<const uint8_t *>(buf);
            const uint8_t *end      = &ptr[bytes];

            while (ptr < end)
            {
                // Find dicionary match
                ssize_t lend    = lookup_dict(&locd, &sRoot, ptr, end - ptr);
                if (locd.len >= 4)
                {
                    ptr    += lend;
                    continue;
                }

                // No match - add new word
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
                        if (!(curr = add_child(curr, b)))
                            return STATUS_NO_MEM;

                        // Increment number of hits for each node
                        for ( ; curr != &sRoot; curr = curr->parent)
                            ++ curr->hits;
                    }

                    // Skip repeated characters
                    while ((ptr < end) && (*ptr == b))
                        ++ptr;
                    break;
                }
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

        ssize_t Compressor::lookup_dict(location_t *out, node_t *root, const uint8_t *s, size_t avail)
        {
            out->offset     = -1;
            out->len        = 0;
            out->repeat     = 0;

            node_t *curr        = root;
            size_t i            = 0;

            for (; i < avail; ++i)
            {
                node_t *next        = get_child(curr, s[i]);

                // No more node or length reduced dramatially?
                if ((next == NULL) || (next->hits < (i+1)))
                    break;

                // Move to next node
                curr                = next;
            }

            // Remember the length and offset
            out->offset     = curr->off;
            out->len        = i;

            // Compute the number of repetitions
            for (size_t j = i+1; j < avail; ++j, ++out->repeat)
                if (s[i] != s[j])
                    break;

            return out->len + out->repeat;
        }

        ssize_t Compressor::commit_dict(io::OutMemoryStream *out, location_t *loc, node_t *root, const uint8_t *s)
        {
            // Offset is defined?
            if (loc->offset >= 0)
                return STATUS_OK;

            // Emit the word
            loc->offset         = out->size();
            node_t *curr        = root;
            for (size_t i=0; i<loc->len; ++i)
            {
                curr                = get_child(curr, s[i]);

                // Remember the start position of the word
                if (curr->off < 0)
                    curr->off           = loc->offset;
            }

            // Commit data to buffer
            ssize_t written = out->write(s, loc->len);
            if (written < 0)
                return -written;
            return (written == ssize_t(loc->len)) ? STATUS_OK : STATUS_NO_MEM;
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

        status_t Compressor::emit_uint(io::OutBitStream *obs, size_t value, size_t initial, size_t stepping)
        {
            status_t res;
            size_t bits     = initial;

            while (true)
            {
                size_t max  = (1 << bits);
                if (value < max)
                    break;
                if ((res = obs->writeb(true)) != STATUS_OK)
                    return res;

                value      -= max;
                bits       += stepping;
            }

            if ((res = obs->writeb(false)) != STATUS_OK)
                return res;

            return (bits > 0) ? obs->writev(value, bits) : STATUS_OK;
        }

        size_t Compressor::calc_repeats(const uint8_t *head, const uint8_t *tail)
        {
            uint8_t b       = *head;
            ssize_t nrep    = 0;
            for (const uint8_t *ptr=head + 1; (ptr < tail) && (*ptr == b); ++ptr)
                ++nrep;

            return nrep;
        }

        status_t Compressor::compress()
        {
            status_t res;
            IF_TRACE(
                LSPString tmp;
            )

            // Wrap data with out bit stream
            io::OutBitStream obs;
            if ((res = obs.wrap(&sCommands)) != STATUS_OK)
                return res;

            // Do main compression loop
            buffer_t buf;
            if ((res = buf.init(BUFFER_CAPACITY)) != STATUS_OK)
                return res;

            ssize_t offset = 0, length = 0, rep = 0;

            for (size_t i=0, n=vEntries.size(); i<n; ++i)
            {
                raw_resource_t *r = vEntries.uget(i);
                if ((r == NULL) || (r->type != RES_FILE))
                    continue;

                lsp_trace("  compressing entry: %s", r->name);
                const uint8_t *head = sData.data();
                head               += r->offset;
                const uint8_t *tail = &head[r->length];
                r->offset           = sCommands.size();

                size_t octets   = 0;
                size_t replays  = 0;
                size_t repeats  = 0;

                while (head < tail)
                {
                    length      = buf.lookup(&offset, head, tail-head);

                    if (length >= 2) // Prefer buffer over dictionary
                    {
                        rep         = calc_repeats(&head[length], tail);

                        // Offset
                        if ((res = emit_uint(&obs, offset, 7, 7)) != STATUS_OK)
                            break;
                        // Length
                        if ((res = emit_uint(&obs, length - 2, 5, 5)) != STATUS_OK)
                            break;
                        // Repeat
                        if ((res = emit_uint(&obs, rep, 0, 4)) != STATUS_OK)
                            break;
                        ++ replays;
                        if (rep)
                            ++ repeats;

                        buf.append(head, length);
                        head           += length;
                    }
                    else
                    {
                        // OCTET
                        rep         = calc_repeats(head, tail);
                        length      = lsp_min(rep, 4) + 1;

                        // Offset
                        if ((res = emit_uint(&obs, buf.size() + rep, 7, 7)) != STATUS_OK)
                            break;
                        // Value
                        if ((res = obs.writev(*head)) != STATUS_OK)
                            break;
                        // Append to buffer and proceed
                        buf.append(head, length);
                        head           += length;
                        ++ octets;
                    }
                }

                // Flush the bit sequence
                if (res != STATUS_OK)
                    break;

                // Compute number of bytes for compressed item
                r->cbytes   = sCommands.size() - r->offset;

                lsp_trace("  octets: %d, replays: %d, repeats: %d",
                        int(octets), int(replays), int(repeats));
                lsp_trace("  original size: %d, compressed size: %d, ratio: %.2f",
                        r->length, r->cbytes, float(r->length) / float(r->cbytes));
            }

            size_t csize = sCommands.size() + sBuffer.size();

            lsp_trace("  overall size: %d, compressed size: %d, ratio: %.2f",
                    sData.size(), csize, float(sData.size()) / float(csize));

            return res;
        }

        status_t Compressor::flush()
        {
            return STATUS_OK;
        }
    }
}


