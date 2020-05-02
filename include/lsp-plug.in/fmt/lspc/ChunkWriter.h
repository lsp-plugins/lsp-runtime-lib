/*
 * LSPChunkWriter.h
 *
 *  Created on: 14 янв. 2018 г.
 *      Author: sadko
 */

#ifndef LSP_PLUG_IN_FMT_LSPC_CHUNKWRITER_H_
#define LSP_PLUG_IN_FMT_LSPC_CHUNKWRITER_H_

#include <lsp-plug.in/runtime/version.h>
#include <lsp-plug.in/fmt/lspc/ChunkAccessor.h>
#include <lsp-plug.in/fmt/lspc/lspc.h>

namespace lsp
{
    namespace lspc
    {
        class ChunkWriter: public ChunkAccessor
        {
            private:
                ChunkWriter & operator = (const ChunkWriter &);

            protected:
                friend class File;

                enum flush_t
                {
                    F_FORCE         = 1 << 0,
                    F_LAST          = 1 << 1
                };

            protected:
                size_t              nChunksOut;

            protected:
                status_t            do_flush(size_t flags);

            protected:
                explicit ChunkWriter(Resource *fd, uint32_t magic);
    
            public:
                virtual ~ChunkWriter();

            public:
                /**
                 * Write chunk data header to LSPC chunk.
                 * @param buf buffer containing header
                 * @return status of operation
                 */
                virtual status_t    write_header(const void *buf);

                /**
                 * Write data to LSPC chunk
                 * @param buf buffer to write
                 * @param count number of bytes to write
                 * @return status of operation
                 */
                virtual status_t    write(const void *buf, size_t count);

                /**
                 * Flush all buffers to file
                 * @return status of operation
                 */
                virtual status_t    flush();

                /**
                 * Close chunk writer
                 * @return status of operation
                 */
                virtual status_t    close();
        };
    }

} /* namespace lsp */

#endif /* LSP_PLUG_IN_FMT_LSPC_CHUNKWRITER_H_ */
