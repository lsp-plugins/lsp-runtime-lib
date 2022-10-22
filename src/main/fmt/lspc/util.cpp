/*
 * Copyright (C) 2022 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2022 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-runtime-lib
 * Created on: 22 окт. 2022 г.
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
#include <lsp-plug.in/common/types.h>
#include <lsp-plug.in/fmt/lspc/util.h>
#include <lsp-plug.in/fmt/lspc/ChunkWriterStream.h>
#include <lsp-plug.in/io/InFileStream.h>
#include <lsp-plug.in/io/InMemoryStream.h>
#include <lsp-plug.in/stdlib/stdlib.h>
#include <lsp-plug.in/stdlib/string.h>

namespace lsp
{
    namespace lspc
    {
        path_entry_t *alloc_path_entry(const char *path, size_t flags, chunk_id_t reference_id)
        {
            if (path == NULL)
                return NULL;

            // Allocate entry
            size_t path_len     = strlen(path) + 1;
            size_t hdr_size     = align_size(sizeof(path), sizeof(umword_t));
            size_t path_size    = align_size(path_len, sizeof(umword_t));

            uint8_t *bytes      = static_cast<uint8_t *>(malloc(hdr_size + path_size));
            if (bytes == NULL)
                return NULL;

            path_entry_t *res   = reinterpret_cast<path_entry_t *>(bytes);
            char *str           = reinterpret_cast<char *>(&bytes[hdr_size]);

            // Fill entry with data
            res->path           = str;
            res->flags          = flags;
            res->chunk_id       = reference_id;
            memcpy(str, path, path_len);

            return res;
        }

        path_entry_t *alloc_path_entry(const io::Path *path, size_t flags, chunk_id_t reference_id)
        {
            if (path == NULL)
                return NULL;
            return alloc_path_entry(path->as_utf8(), flags, reference_id);
        }

        path_entry_t *alloc_path_entry(const LSPString *path, size_t flags, chunk_id_t reference_id)
        {
            if (path == NULL)
                return NULL;
            return alloc_path_entry(path->get_utf8(), flags, reference_id);
        }

        void free_path_entry(path_entry_t * path)
        {
            if (path != NULL)
                free(path);
        }

        status_t write_path_entry(chunk_id_t *chunk_id, File *file, const path_entry_t *path)
        {
            if (path == NULL)
                return STATUS_BAD_ARGUMENTS;

            // Create chunk and remember it's size
            status_t res;
            lspc::ChunkWriter *cw = file->write_chunk(LSPC_CHUNK_AUDIO);
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
            if ((res = cw->flush()) != STATUS_OK)
                return res;
            if ((res = cw->close()) != STATUS_OK)
                return res;

            // Return result
            if (chunk_id != NULL)
                *chunk_id           = res_chunk_id;

            return STATUS_OK;
        }

        status_t write_path_entry(chunk_id_t *chunk_id, File *file, const char *path, size_t flags, chunk_id_t reference_id)
        {
            if (path == NULL)
                return STATUS_BAD_ARGUMENTS;

            path_entry_t pe;
            pe.path     = path;
            pe.flags    = flags;
            pe.chunk_id = reference_id;

            return write_path_entry(chunk_id, file, &pe);
        }

        status_t write_path_entry(chunk_id_t *chunk_id, File *file, const io::Path *path, size_t flags, chunk_id_t reference_id)
        {
            if (path == NULL)
                return STATUS_BAD_ARGUMENTS;

            path_entry_t pe;
            pe.path     = path->as_utf8();
            pe.flags    = flags;
            pe.chunk_id = reference_id;

            return write_path_entry(chunk_id, file, &pe);
        }

        status_t write_path_entry(chunk_id_t *chunk_id, File *file, const LSPString *path, size_t flags, chunk_id_t reference_id)
        {
            if (path == NULL)
                return STATUS_BAD_ARGUMENTS;

            path_entry_t pe;
            pe.path     = path->get_utf8();
            pe.flags    = flags;
            pe.chunk_id = reference_id;

            return write_path_entry(chunk_id, file, &pe);
        }

        status_t write_config_entry(chunk_id_t *chunk_id, File *file, const char *path, size_t buf_size)
        {
            io::InFileStream is;
            status_t res = is.open(path);
            if (res != STATUS_OK)
                return res;
            res = write_config_entry(chunk_id, file, &is, buf_size);
            status_t res2 = is.close();
            return (res != STATUS_OK) ? res : res2;
        }

        status_t write_config_entry(chunk_id_t *chunk_id, File *file, const io::Path *path, size_t buf_size)
        {
            io::InFileStream is;
            status_t res = is.open(path);
            if (res != STATUS_OK)
                return res;
            res = write_config_entry(chunk_id, file, &is, buf_size);
            status_t res2 = is.close();
            return (res != STATUS_OK) ? res : res2;
        }

        status_t write_config_entry(chunk_id_t *chunk_id, File *file, const LSPString *path, size_t buf_size)
        {
            io::InFileStream is;
            status_t res = is.open(path);
            if (res != STATUS_OK)
                return res;
            res = write_config_entry(chunk_id, file, &is, buf_size);
            status_t res2 = is.close();
            return (res != STATUS_OK) ? res : res2;
        }

        status_t write_config_entry(chunk_id_t *chunk_id, File *file, io::IInStream *is, size_t buf_size)
        {
            lspc::ChunkWriter *wr = NULL;
            chunk_id_t res_chunk_id;

            // Create configuration entry and write header
            status_t res = write_config_entry(&res_chunk_id, file, &wr);
            if (res != STATUS_OK)
                return res;
            else if (wr == NULL)
                return STATUS_NO_MEM;
            lsp_finally { delete wr; };

            // Write data to configuration entry
            io::IOutStream *os = wr->stream();
            res = is->sink(os, buf_size);
            if (res == STATUS_OK)
                res = os->flush();

            // Close stream, writer and exit
            status_t res2 = os->close();
            if (res == STATUS_OK)
                res = res2;
            res2 = wr->close();
            if (res == STATUS_OK)
                res = res2;

            // Return result
            if (res != STATUS_OK)
                return res;
            if (chunk_id != NULL)
                *chunk_id = res_chunk_id;
            return STATUS_OK;
        }

        status_t write_config_entry(chunk_id_t *chunk_id, File *file, io::IOutStream **os)
        {
            lspc::ChunkWriter *wr = NULL;
            chunk_id_t res_chunk_id;

            // We should have valid pointer to store the result
            if (os == NULL)
                return STATUS_BAD_ARGUMENTS;

            // Create configuration entry and write header
            status_t res = write_config_entry(&res_chunk_id, file, &wr);
            if (res != STATUS_OK)
                return res;
            else if (wr == NULL)
                return STATUS_NO_MEM;

            // Allocate stream pointer and return it
            ChunkWriterStream *cws = new ChunkWriterStream(wr, true);
            if (cws == NULL)
            {
                wr->close();
                delete wr;
                return STATUS_NO_MEM;
            }

            // Return result
            if (chunk_id != NULL)
                *chunk_id = res_chunk_id;
            *os     = cws;
            return STATUS_OK;
        }

        status_t write_config_entry_data(chunk_id_t *chunk_id, File *file, const char *data, size_t buf_size)
        {
            if (data == NULL)
                return STATUS_BAD_ARGUMENTS;
            return write_config_entry_data(chunk_id, file, data, strlen(data));
        }

        status_t write_config_entry_data(chunk_id_t *chunk_id, File *file, const LSPString *data, size_t buf_size)
        {
            if (data == NULL)
                return STATUS_BAD_ARGUMENTS;
            return write_config_entry_data(chunk_id, file, data->get_utf8());
        }

        status_t write_config_entry_data(chunk_id_t *chunk_id, File *file, const void *data, size_t bytes, size_t buf_size)
        {
            if (data == NULL)
                return STATUS_BAD_ARGUMENTS;

            io::InMemoryStream is;
            is.wrap(data, bytes);
            status_t res    = write_config_entry(chunk_id, file, &is, buf_size);
            status_t res2   = is.close();
            return (res != STATUS_OK) ? res : res2;
        }

        status_t write_config_entry(chunk_id_t *chunk_id, File *file, lspc::ChunkWriter **writer)
        {
            if (writer == NULL)
                return STATUS_BAD_ARGUMENTS;

            // Create chunk and remember it's size
            status_t res;
            lspc::ChunkWriter *cw = file->write_chunk(LSPC_CHUNK_TEXT_CONFIG);
            if (cw == NULL)
                return STATUS_BAD_STATE;
            lsp_finally {
                if (cw != NULL)
                    delete cw;
            };

            // Form the chunk header
            chunk_text_config_t hdr;
            bzero(&hdr, sizeof(hdr));
            hdr.common.size     = sizeof(hdr);
            hdr.common.version  = 0;

            // Write header of the chunk
            if ((res = cw->write_header(&hdr)) != STATUS_OK)
                return res;

            // Return result
            if (chunk_id != NULL)
                *chunk_id = cw->unique_id();
            *writer = cw;
            cw      = NULL;

            return STATUS_OK;
        }

    } /* namespace lspc */
} /* namespace lsp */


