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
#include <lsp-plug.in/common/types.h>
#include <lsp-plug.in/fmt/lspc/util/config.h>
#include <lsp-plug.in/io/InFileStream.h>
#include <lsp-plug.in/io/InMemoryStream.h>

namespace lsp
{
    namespace lspc
    {
        LSP_RUNTIME_LIB_PUBLIC
        status_t write_config(chunk_id_t *chunk_id, File *file, const char *path, size_t buf_size)
        {
            io::InFileStream is;
            status_t res = is.open(path);
            if (res != STATUS_OK)
                return res;
            res = write_config(chunk_id, file, &is, buf_size);
            status_t res2 = is.close();
            return (res != STATUS_OK) ? res : res2;
        }

        LSP_RUNTIME_LIB_PUBLIC
        status_t write_config(chunk_id_t *chunk_id, File *file, const io::Path *path, size_t buf_size)
        {
            io::InFileStream is;
            status_t res = is.open(path);
            if (res != STATUS_OK)
                return res;
            res = write_config(chunk_id, file, &is, buf_size);
            status_t res2 = is.close();
            return (res != STATUS_OK) ? res : res2;
        }

        LSP_RUNTIME_LIB_PUBLIC
        status_t write_config(chunk_id_t *chunk_id, File *file, const LSPString *path, size_t buf_size)
        {
            io::InFileStream is;
            status_t res = is.open(path);
            if (res != STATUS_OK)
                return res;
            res = write_config(chunk_id, file, &is, buf_size);
            status_t res2 = is.close();
            return (res != STATUS_OK) ? res : res2;
        }

        LSP_RUNTIME_LIB_PUBLIC
        status_t write_config(chunk_id_t *chunk_id, File *file, io::IInStream *is, size_t buf_size)
        {
            lspc::ChunkWriter *wr = NULL;
            chunk_id_t res_chunk_id;

            // Create configuration entry and write header
            status_t res = write_config(&res_chunk_id, file, &wr);
            if (res != STATUS_OK)
                return res;
            else if (wr == NULL)
                return STATUS_NO_MEM;
            lsp_finally { delete wr; };

            // Write data to configuration entry
            io::IOutStream *os = wr->stream();
            wssize_t written = is->sink(os, buf_size);
            res = (written < 0) ? -written : STATUS_OK;

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

        LSP_RUNTIME_LIB_PUBLIC
        status_t write_config(chunk_id_t *chunk_id, File *file, io::IOutStream **os)
        {
            lspc::ChunkWriter *wr = NULL;
            chunk_id_t res_chunk_id;

            // We should have valid pointer to store the result
            if (os == NULL)
                return STATUS_BAD_ARGUMENTS;

            // Create configuration entry and write header
            status_t res = write_config(&res_chunk_id, file, &wr);
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

        LSP_RUNTIME_LIB_PUBLIC
        status_t write_config_data(chunk_id_t *chunk_id, File *file, const char *data, size_t buf_size)
        {
            if (data == NULL)
                return STATUS_BAD_ARGUMENTS;
            return write_config_data(chunk_id, file, data, strlen(data));
        }

        LSP_RUNTIME_LIB_PUBLIC
        status_t write_config_data(chunk_id_t *chunk_id, File *file, const LSPString *data, size_t buf_size)
        {
            if (data == NULL)
                return STATUS_BAD_ARGUMENTS;
            return write_config_data(chunk_id, file, data->get_utf8());
        }

        LSP_RUNTIME_LIB_PUBLIC
        status_t write_config_data(chunk_id_t *chunk_id, File *file, const void *data, size_t bytes, size_t buf_size)
        {
            if (data == NULL)
                return STATUS_BAD_ARGUMENTS;

            io::InMemoryStream is;
            is.wrap(data, bytes);
            status_t res    = write_config(chunk_id, file, &is, buf_size);
            status_t res2   = is.close();
            return (res != STATUS_OK) ? res : res2;
        }

        LSP_RUNTIME_LIB_PUBLIC
        status_t write_config(chunk_id_t *chunk_id, File *file, lspc::ChunkWriter **writer)
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



