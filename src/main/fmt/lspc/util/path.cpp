/*
 * Copyright (C) 2022 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2022 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-runtime-lib
 * Created on: 24 окт. 2022 г.
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

#include <lsp-plug.in/common/alloc.h>
#include <lsp-plug.in/common/endian.h>
#include <lsp-plug.in/fmt/lspc/ChunkWriter.h>
#include <lsp-plug.in/fmt/lspc/util/path.h>
#include <lsp-plug.in/stdlib/stdlib.h>
#include <lsp-plug.in/stdlib/string.h>

namespace lsp
{
    namespace lspc
    {
        //---------------------------------------------------------------------
        // Allocation/deallocation
        LSP_RUNTIME_LIB_PUBLIC
        path_entry_t *alloc_path(size_t path_len)
        {
            // Allocate entry
            size_t hdr_size     = align_size(sizeof(path_entry_t), sizeof(umword_t));
            size_t path_size    = align_size(path_len + 1, sizeof(umword_t));

            uint8_t *bytes      = static_cast<uint8_t *>(malloc(hdr_size + path_size));
            if (bytes == NULL)
                return NULL;

            path_entry_t *res   = reinterpret_cast<path_entry_t *>(bytes);
            char *str           = reinterpret_cast<char *>(&bytes[hdr_size]);

            // Fill entry with data
            res->path           = str;
            res->flags          = 0;
            res->chunk_id       = 0;

            return res;
        }

        LSP_RUNTIME_LIB_PUBLIC
        path_entry_t *alloc_path(const char *path, size_t flags, chunk_id_t reference_id)
        {
            if (path == NULL)
                return NULL;

            // Allocate entry
            size_t path_len     = strlen(path) + 1;
            path_entry_t *res   = alloc_path(path_len);
            if (res == NULL)
                return NULL;

            // Fill entry with data
            res->flags          = flags;
            res->chunk_id       = reference_id;
            memcpy(res->path, path, path_len);

            return res;
        }

        LSP_RUNTIME_LIB_PUBLIC
        path_entry_t *alloc_path(const io::Path *path, size_t flags, chunk_id_t reference_id)
        {
            if (path == NULL)
                return NULL;
            return alloc_path(path->as_utf8(), flags, reference_id);
        }

        LSP_RUNTIME_LIB_PUBLIC
        path_entry_t *alloc_path(const LSPString *path, size_t flags, chunk_id_t reference_id)
        {
            if (path == NULL)
                return NULL;
            return alloc_path(path->get_utf8(), flags, reference_id);
        }

        LSP_RUNTIME_LIB_PUBLIC
        void free_path(path_entry_t * path)
        {
            if (path != NULL)
                free(path);
        }

        //---------------------------------------------------------------------
        // Write operations
        LSP_RUNTIME_LIB_PUBLIC
        status_t write_path(chunk_id_t *chunk_id, File *file, const path_entry_t *path)
        {
            if (path == NULL)
                return STATUS_BAD_ARGUMENTS;

            // Create chunk and remember it's size
            status_t res;
            lspc::ChunkWriter *cw = file->write_chunk(LSPC_CHUNK_PATH);
            if (cw == NULL)
                return STATUS_BAD_STATE;
            lsp_finally { delete cw; };
            chunk_id_t res_chunk_id = cw->unique_id();

            // Create header
            size_t path_size    = strlen(path->path);
            if (path_size > UINT16_MAX)
                return STATUS_OVERFLOW;

            chunk_path_t cp;
            bzero(&cp, sizeof(cp));

            cp.common.size      = sizeof(cp);
            cp.common.version   = 0;
            cp.path_size        = path_size;
            cp.flags            = path->flags;
            cp.chunk_id         = path->chunk_id;

            // Convert endianess
            cp.path_size       = CPU_TO_BE(cp.path_size);
            cp.flags           = CPU_TO_BE(cp.flags);
            cp.chunk_id        = CPU_TO_BE(cp.chunk_id);

            if ((res = cw->write_header(&cp)) != STATUS_OK)
                return res;
            if ((res = cw->write(path->path, path_size)) != STATUS_OK)
                return res;
            if ((res = cw->close()) != STATUS_OK)
                return res;

            // Return result
            if (chunk_id != NULL)
                *chunk_id           = res_chunk_id;

            return STATUS_OK;
        }

        LSP_RUNTIME_LIB_PUBLIC
        status_t write_path(chunk_id_t *chunk_id, File *file, const char *path, size_t flags, chunk_id_t reference_id)
        {
            if (path == NULL)
                return STATUS_BAD_ARGUMENTS;

            path_entry_t pe;
            pe.path     = const_cast<char *>(path);
            pe.flags    = flags;
            pe.chunk_id = reference_id;

            return write_path(chunk_id, file, &pe);
        }

        LSP_RUNTIME_LIB_PUBLIC
        status_t write_path(chunk_id_t *chunk_id, File *file, const io::Path *path, size_t flags, chunk_id_t reference_id)
        {
            if (path == NULL)
                return STATUS_BAD_ARGUMENTS;

            path_entry_t pe;
            pe.path     = const_cast<char *>(path->as_utf8());
            pe.flags    = flags;
            pe.chunk_id = reference_id;

            return write_path(chunk_id, file, &pe);
        }

        LSP_RUNTIME_LIB_PUBLIC
        status_t write_path(chunk_id_t *chunk_id, File *file, const LSPString *path, size_t flags, chunk_id_t reference_id)
        {
            if (path == NULL)
                return STATUS_BAD_ARGUMENTS;

            path_entry_t pe;
            pe.path     = const_cast<char *>(path->get_utf8());
            pe.flags    = flags;
            pe.chunk_id = reference_id;

            return write_path(chunk_id, file, &pe);
        }
    } /* namespace lspc */
} /* namespace lsp */

