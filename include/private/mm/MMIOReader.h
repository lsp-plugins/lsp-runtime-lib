/*
 * MMIOReader.h
 *
 *  Created on: 24 апр. 2020 г.
 *      Author: sadko
 */

#ifndef PRIVATE_MM_MMIOREADER_H_
#define PRIVATE_MM_MMIOREADER_H_

#include <lsp-plug.in/runtime/version.h>
#include <lsp-plug.in/common/types.h>
#include <lsp-plug.in/common/status.h>
#include <lsp-plug.in/runtime/LSPString.h>

#ifndef USE_LIBSNDFILE

#include <mmsystem.h>
#include <mmreg.h>

namespace lsp
{
    namespace mm
    {
        /**
         * Audio file reader using MMIO subsystem
         */
        class MMIOReader
        {
            private:
                MMIOReader & operator = (const MMIOReader &);

            protected:
                HMMIO               hMMIO;
                wsize_t             nReadPos;
                wssize_t            nFrames;
                bool                bSeekable;
                WAVEFORMATEX       *pFormat;
                MMCKINFO            ckRiff;
                MMCKINFO            ckData;

            public:
                explicit MMIOReader();
                ~MMIOReader();

            public:
                /**
                 * Check whether reader is seekable
                 * @return true if reader is seekable
                 */
                inline bool seekable() const                { return bSeekable; }

                /**
                 * Get input format descriptor
                 * @return input format descriptor
                 */
                inline WAVEFORMATEX    *format()            { return pFormat; }

                /**
                 * Get actual number of frames
                 * @return number of frames or negative value if length is unknown
                 */
                inline wssize_t         frames() const      { return nFrames; }

                /**
                 * Open audio file for reading
                 * @param path path to the file
                 * @return status of operation
                 */
                status_t    open(const LSPString *path);

                /**
                 * Seek to the specified location
                 * @param offset offset in bytes from the beginning of data chunk
                 * @return actual offset in bytes from the beginning of data chunk
                 */
                wssize_t    seek(wsize_t offset);

                /**
                 * Read number of bytes into buffer
                 * @param buf pointer to buffer to store data
                 * @param count number of bytes to read
                 * @return actual bytes read or negative error code
                 */
                ssize_t     read(void *buf, size_t count);

                /**
                 * Close the file
                 * @return status of operation
                 */
                status_t    close(status_t code = STATUS_OK);
        };
    
    } /* namespace mm */
} /* namespace lsp */

#endif /* USE_LIBSNDFILE */

#endif /* PRIVATE_MM_MMIOREADER_H_ */
