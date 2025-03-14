/*
 * Copyright (C) 2023 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2023 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-runtime-lib
 * Created on: 8 мар. 2019 г.
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

#ifndef LSP_PLUG_IN_IO_CHARSETENCODER_H_
#define LSP_PLUG_IN_IO_CHARSETENCODER_H_

#include <lsp-plug.in/runtime/version.h>
#include <lsp-plug.in/common/status.h>
#include <lsp-plug.in/io/charset.h>
#include <lsp-plug.in/io/File.h>
#include <lsp-plug.in/io/IInSequence.h>
#include <lsp-plug.in/io/IOutStream.h>

namespace lsp
{
    namespace io
    {
        /**
         * Character set encoder, from UTF-32 to any character set
         * Important! fetch() operations do not always return blocks
         * aligned to the character range.
         */
        class CharsetEncoder
        {
            protected:
                uint8_t        *bBuffer;        // Temporary buffer for storing input byte sequence
                uint8_t        *bBufHead;       // Buffer head
                uint8_t        *bBufTail;       // Buffer tail

                lsp_wchar_t    *cBuffer;        // Temporary buffer for storing UTF-16 code points
                lsp_wchar_t    *cBufHead;       // Character buffer head
                lsp_wchar_t    *cBufTail;       // Character buffer tail

#if defined(PLATFORM_WINDOWS)
                lsp_utf16_t    *xBuffer;        // Additional translation buffer
                uint32_t        nCodePage;      // Code page
#else
                iconv_t         hIconv;         // iconv handle
#endif /* PLATFORM_WINDOWS */

            private:
                inline size_t   prepare_buffer();
                ssize_t         encode_buffer();

            public:
                explicit CharsetEncoder();
                CharsetEncoder(const CharsetEncoder &) = delete;
                CharsetEncoder(CharsetEncoder &&) = delete;
                ~CharsetEncoder();

                CharsetEncoder &operator = (const CharsetEncoder &) = delete;
                CharsetEncoder &operator = (CharsetEncoder &&) = delete;

            public:
                /**
                 * Initialize character-set decoder
                 * @param charset character set name, NULL for default native character set
                 * @return status of operation
                 */
                status_t    init(const char *charset = NULL);

                /**
                 * Close decoder and free all resources
                 */
                void        close();

                /**
                 * Fill internal buffer with character data
                 * @param buf source buffer
                 * @param count buffer size
                 * @return number of characters processed or negative error code
                 */
                ssize_t     fill(const lsp_wchar_t *buf, size_t count);

                /**
                 * Fill internal buffer with ASCII character data
                 * @param buf source buffer
                 * @param count buffer size
                 * @return number of characters processed or negative error code
                 */
                ssize_t     fill(const char *buf, size_t count);

                /**
                 * Fill internal buffer with character data from sequence
                 * @param in input sequence to perform character reading
                 * @param count maximum number of characters to process, 0 means maximum possible value
                 * @return number of characters processed or negative error code
                 */
                ssize_t     fill(IInSequence *in, size_t count = 0);

                /**
                 * Fill internal buffer with one single character
                 * @param ch character to add to buffer
                 * @return positive number on success 0 if no place in buffer or negative status of operation
                 */
                ssize_t     fill(lsp_wchar_t ch);

                /**
                 * Fill internal buffer with character data from string
                 * @param in input string to perform character reading
                 * @param first the index of first character to take from string
                 * @param last the index of first character that won't be taken
                 * @return number of characters processed or negative error code
                 */
                ssize_t         fill(const LSPString *in, size_t first, size_t last);
                inline ssize_t  fill(const LSPString *in, size_t first) { return (in == NULL) ? -STATUS_BAD_ARGUMENTS : fill(in, first, in->length()); };
                inline ssize_t  fill(const LSPString *in) { return (in == NULL) ? -STATUS_BAD_ARGUMENTS : fill(in, 0, in->length()); };

                /**
                 * Fetch output byte data into buffer
                 * @param buf buffer to perform fetch
                 * @param count maximum number of bytes to fetch
                 * @return number of bytes fetched or negative error code
                 */
                ssize_t     fetch(void *buf, size_t count);

                /**
                 * Fetch output byte data into file object
                 * @param out file to store the fetch result
                 * @param count maximum number of bytes to fetch, 0 means maximum possible value
                 * @return number of bytes fetched or negative error code
                 */
                ssize_t     fetch(File *out, size_t count = 0);

                /**
                 * Fetch output byte data into output stream
                 * @param out buffer to store the fetch result
                 * @param count maximum number of bytes to fetch, 0 means maximum possible value
                 * @return number of bytes fetched or negative error code
                 */
                ssize_t     fetch(IOutStream *out, size_t count = 0);
        };
    
    } /* namespace io */
} /* namespace lsp */

#endif /* LSP_PLUG_IN_IO_CHARSETENCODER_H_ */
