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

#ifndef LSP_PLUG_IN_FMT_LSPC_UTIL_H_
#define LSP_PLUG_IN_FMT_LSPC_UTIL_H_

#include <lsp-plug.in/runtime/version.h>

#include <lsp-plug.in/common/status.h>
#include <lsp-plug.in/fmt/lspc/ChunkWriter.h>
#include <lsp-plug.in/fmt/lspc/File.h>
#include <lsp-plug.in/io/IInStream.h>
#include <lsp-plug.in/io/Path.h>
#include <lsp-plug.in/runtime/LSPString.h>

namespace lsp
{
    namespace lspc
    {
        /**
         * Allocate path entry and fill it's fields
         * @param path relative path of the file
         * @param flags path entry flags
         * @param reference_id refere
         * @return pointer to allocated path entry or NULL if error occurred
         */
        path_entry_t *alloc_path_entry(const char *path, size_t flags, chunk_id_t reference_id);
        path_entry_t *alloc_path_entry(const io::Path *path, size_t flags, chunk_id_t reference_id);
        path_entry_t *alloc_path_entry(const LSPString *path, size_t flags, chunk_id_t reference_id);

        /**
         * Free previously allocated path entry
         * @param path path entry to free
         */
        void free_path_entry(path_entry_t * path);

        /**
         * Write path entry to LSPC file
         * @param chunk_id pointer to return chunk identifier, can be NULL
         * @param file the LSPC file to create chunk
         * @param path path entry
         * @return status of operation
         */
        status_t write_path_entry(chunk_id_t *chunk_id, File *file, const path_entry_t *path);

        /**
         * Write path entry to LSPC file
         * @param chunk_id pointer to store allocated chunk identifier, can be NULL
         * @param file the LSPC file to write the chunk
         * @param path the path string
         * @param flags path flags
         * @param reference_id the identifier of chunk referenced by the path entry
         * @return status of operation
         */
        status_t write_path_entry(chunk_id_t *chunk_id, File *file, const char *path, size_t flags, chunk_id_t reference_id);
        status_t write_path_entry(chunk_id_t *chunk_id, File *file, const io::Path *path, size_t flags, chunk_id_t reference_id);
        status_t write_path_entry(chunk_id_t *chunk_id, File *file, const LSPString *path, size_t flags, chunk_id_t reference_id);

        /**
         * Write configuration entry to the LSPC file
         * @param chunk_id pointer to store allocated chunk identifier, can be NULL
         * @param file the LSPC file to write the chunk
         * @param path the location of configuration file to write
         * @param buf_size size of buffer for I/O operations
         * @return status of operation
         */
        status_t write_config_entry(chunk_id_t *chunk_id, File *file, const char *path, size_t buf_size = 0x1000);
        status_t write_config_entry(chunk_id_t *chunk_id, File *file, const io::Path *path, size_t buf_size = 0x1000);
        status_t write_config_entry(chunk_id_t *chunk_id, File *file, const LSPString *path, size_t buf_size = 0x1000);

        /**
         * Write the contents of the passed stream to the LSPC file as a configuration chunk
         * @param chunk_id pointer to store allocated chunk identifier, can be NULL
         * @param file the LSPC file to write the chunk
         * @param is input stream that provides data to write
         * @param buf_size size of buffer for I/O operations
         * @return status of operation
         */
        status_t write_config_entry(chunk_id_t *chunk_id, File *file, io::IInStream *is, size_t buf_size = 0x1000);

        /**
         * Create configuration entry in the LSPC file and return chunk writer that will allow to write to the entry
         * @param chunk_id pointer to store allocated chunk identifier, can be NULL
         * @param file the LSPC file to write the chunk
         * @param writer pointer to store the created chunk writer that should be deleted after use
         * @return status of operation
         */
        status_t write_config_entry(chunk_id_t *chunk_id, File *file, lspc::ChunkWriter **writer);

        /**
         * Create configuration entry in the LSPC file and return chunk writer that will allow to write to the entry
         * @param chunk_id pointer to store allocated chunk identifier, can be NULL
         * @param file the LSPC file to write the chunk
         * @param os pointer to store the created output stream that should be deleted after use
         * @param buf_size size of buffer for I/O operations
         * @return status of operation
         */
        status_t write_config_entry(chunk_id_t *chunk_id, File *file, io::IOutStream **os);

        /**
         * Write configuration entry data to the LSPC file
         * @param chunk_id pointer to store allocated chunk identifier, can be NULL
         * @param file the LSPC file to write the chunk
         * @param data the text configuration data to write to the file
         * @param buf_size size of buffer for I/O operations
         * @return status of operation
         */
        status_t write_config_entry_data(chunk_id_t *chunk_id, File *file, const char *data, size_t buf_size = 0x1000);
        status_t write_config_entry_data(chunk_id_t *chunk_id, File *file, const LSPString *data, size_t buf_size = 0x1000);

        /**
         * Write configuration entry data to the LSPC file
         * @param chunk_id pointer to store allocated chunk identifier, can be NULL
         * @param file the LSPC file to write the chunk
         * @param data the binary data to write to the file
         * @param bytes the size of data in bytes
         * @param buf_size size of buffer for I/O operations
         * @return status of operation
         */
        status_t write_config_entry_data(chunk_id_t *chunk_id, File *file, const void *data, size_t bytes, size_t buf_size = 0x1000);
    } /* namespace lspc */
} /* namespace lsp */

#endif /* LSP_PLUG_IN_FMT_LSPC_UTIL_H_ */
