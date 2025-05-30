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

#ifndef LSP_PLUG_IN_IO_CHARSETDECODER_H_
#define LSP_PLUG_IN_IO_CHARSETDECODER_H_

#include <lsp-plug.in/runtime/version.h>
#include <lsp-plug.in/runtime/LSPString.h>
#include <lsp-plug.in/common/status.h>
#include <lsp-plug.in/io/charset.h>
#include <lsp-plug.in/io/File.h>
#include <lsp-plug.in/io/IInStream.h>
#include <lsp-plug.in/io/IOutSequence.h>

namespace lsp
{
    namespace io
    {
        /**
         * Character set decoder, from any character set to UTF-32
         * For most methods utilizes internal buffer.
         * Very suitable for streaming processing.
         */
        class CharsetDecoder
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
                ssize_t         decode_buffer();

            public:
                explicit CharsetDecoder();
                CharsetDecoder(const CharsetDecoder &) = delete;
                CharsetDecoder(CharsetDecoder &&) = delete;
                ~CharsetDecoder();

                CharsetDecoder & operator = (const CharsetDecoder &) = delete;
                CharsetDecoder & operator = (CharsetDecoder &&) = delete;

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
                 * Fetch a single character
                 * @return single character to fetch
                 */
                lsp_swchar_t fetch();

                /**
                 * Fetch decoded characters into output buffer
                 * @param outbuf output buffer to perform fetch
                 * @param count the size of output buffer
                 * @return number of characters fetched or negative error code
                 */
                ssize_t     fetch(lsp_wchar_t *outbuf, size_t count);

                /**
                 * Fetch decoded characters directly into string
                 * @param out output string to perform fetch
                 * @param count the maximum possible amount of characters to fetch, 0 means maximum possible value
                 * @return number of characters fetched or negative error code
                 */
                ssize_t     fetch(LSPString *out, size_t count = 0);

                /**
                 * Fetch decoded characters directly into output sequence
                 * @param out output sequence
                 * @param count maximum number of characters to fetch, 0 means maximum possible value
                 * @return number of characters fetched or negative status of operation
                 */
                ssize_t     fetch(IOutSequence *out, size_t count = 0);

                /**
                 * Fill the internal byte buffer with additional data for decoding
                 * @param buf source buffer with data
                 * @param count size of the buffer
                 * @return number of bytes added to internal buffer
                 */
                ssize_t     fill(const void *buf, size_t count);

                /**
                 * Fill the internal byte buffer with additional data directly from file
                 * @param fd file to read data
                 * @param count maximum number of bytes to read, 0 means maximum possible value
                 * @return number of bytes read into internal buffer
                 */
                ssize_t     fill(File *fd, size_t count = 0);

                /**
                 * Fill the internal byte buffer with additional data directly from input stream
                 * @param is input stream to read data
                 * @param count maximum number of bytes to read, 0 means maximum possible value
                 * @return number of bytes read into internal buffer
                 */
                ssize_t     fill(IInStream *is, size_t count = 0);
        };
    
    } /* namespace io */
} /* namespace lsp */

#endif /* LSP_PLUG_IN_IO_CHARSETDECODER_H_ */
