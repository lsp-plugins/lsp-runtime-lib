/*
 * LSPCFile.h
 *
 *  Created on: 14 янв. 2018 г.
 *      Author: sadko
 */

#ifndef LSP_PLUG_IN_FMT_LSPC_FILE_H_
#define LSP_PLUG_IN_FMT_LSPC_FILE_H_

#include <lsp-plug.in/runtime/version.h>
#include <lsp-plug.in/runtime/LSPString.h>
#include <lsp-plug.in/fmt/lspc/ChunkReader.h>
#include <lsp-plug.in/fmt/lspc/ChunkWriter.h>
#include <lsp-plug.in/io/Path.h>

namespace lsp
{
    namespace lspc
    {
        class File
        {
            protected:
                friend class ChunkReader;
                friend class ChunkWriter;

            private:
                File & operator = (const File &);

            protected:
                Resource       *pFile;      // Shared resource
                bool                bWrite;     // Read/Write mode
                size_t              nHdrSize;   // Size of header

            protected:
                Resource       *create_resource(fhandle_t fd);

            public:
                explicit File();
                virtual ~File();

            public:
                /** Open file for reading
                 *
                 * @param path location of the file
                 * @return status of operation
                 */
                status_t    open(const char *path);

                /** Open file for reading
                 *
                 * @param path location of the file
                 * @return status of operation
                 */
                status_t    open(const LSPString *path);

                /** Open file for reading
                 *
                 * @param path location of the file
                 * @return status of operation
                 */
                status_t    open(const io::Path *path);

                /** Open file for writing
                 *
                 * @param path location of the file
                 * @return status of operation
                 */
                status_t    create(const char *path);

                /** Open file for reading
                 *
                 * @param path location of the file
                 * @return status of operation
                 */
                status_t    create(const LSPString *path);

                /** Open file for reading
                 *
                 * @param path location of the file
                 * @return status of operation
                 */
                status_t    create(const io::Path *path);

                /** Close the file
                 *
                 * @return status of operation
                 */
                status_t    close();

            public:
                /** Write chunk
                 *
                 * @param magic magic number of the chunk type
                 * @return pointer to chunk writer
                 */
                ChunkWriter     *write_chunk(uint32_t magic);

                /** Read chunk
                 *
                 * @param magic magic number of the chunk type
                 * @return pointer to chunk reader
                 */
                ChunkReader     *read_chunk(uint32_t uid);

                /** Read chunk of specific type
                 *
                 * @param uid unique chunk identifier
                 * @param magic magic number of the chunk type
                 * @return pointer to chunk reader
                 */
                ChunkReader     *read_chunk(uint32_t uid, uint32_t magic);

                /**
                 * Find LSPC chunk in file by magic
                 * @param magic chunk magic
                 * @param id pointer to return chunk number
                 * @param start_id start identifier of chunk
                 * @return pointer to chunk reader
                 */
                ChunkReader     *find_chunk(uint32_t magic, uint32_t *id, uint32_t start_id = 1);

                /**
                 * Find LSPC chunk in file by magic
                 * @param magic chunk magic
                 * @param id pointer to return chunk number
                 * @param start_id start identifier of chunk
                 * @return pointer to chunk reader
                 */
                inline ChunkReader *find_chunk(uint32_t magic, uint32_t start_id = 1) { return find_chunk(magic, NULL, start_id); }
        };
    }

} /* namespace lsp */

#endif /* LSP_PLUG_IN_FMT_LSPC_FILE_H_ */
