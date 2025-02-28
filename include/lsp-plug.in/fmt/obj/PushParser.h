/*
 * Copyright (C) 2025 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2025 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-runtime-lib
 * Created on: 22 апр. 2020 г.
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

#ifndef LSP_PLUG_IN_FMT_OBJ_PUSHPARSER_H_
#define LSP_PLUG_IN_FMT_OBJ_PUSHPARSER_H_

#include <lsp-plug.in/runtime/version.h>
#include <lsp-plug.in/fmt/obj/PullParser.h>
#include <lsp-plug.in/fmt/obj/IObjHandler.h>

namespace lsp
{
    namespace obj
    {
        
        class PushParser
        {
            protected:
                PullParser      sParser;

            public:
                status_t parse_document(IObjHandler *handler);

            public:
                explicit PushParser();
                PushParser(const PushParser &) = delete;
                PushParser(PushParser &&) = delete;
                virtual ~PushParser();
            
                PushParser & operator = (const PushParser &) = delete;
                PushParser & operator = (PushParser &&) = delete;

            public:
                /**
                 * Parse file
                 * @param handler Wavefront Object file handler
                 * @param path UTF-8 path to the file
                 * @param charset character set encoding of the file
                 * @return status of operation
                 */
                status_t            parse_file(IObjHandler *handler, const char *path, const char *charset = NULL);

                /**
                 * Parse file
                 * @param handler Wavefront Object file handler
                 * @param path path to the file
                 * @param charset character set encoding of the file
                 * @return status of operation
                 */
                status_t            parse_file(IObjHandler *handler, const LSPString *path, const char *charset = NULL);

                /**
                 * Parse file
                 * @param handler Wavefront Object file handler
                 * @param path path to the file
                 * @param charset character set encoding of the file
                 * @return status of operation
                 */
                status_t            parse_file(IObjHandler *handler, const io::Path *path, const char *charset = NULL);

                /**
                 * Parse data from input stream
                 * @param handler Wavefront Object file handler
                 * @param is input stream
                 * @param flags wrap flags
                 * @param charset character set
                 * @return status of operation
                 */
                status_t            parse_data(IObjHandler *handler, io::IInStream *is, size_t flags = WRAP_NONE, const char *charset = NULL);

                /**
                 * Parse data from native encoded string
                 * @param handler Wavefront Object file handler
                 * @param str data input
                 * @param charset character set
                 * @return status of operation
                 */
                status_t            parse_data(IObjHandler *handler, const char *str, const char *charset = NULL);

                /**
                 * Parse data from text string
                 * @param handler Wavefront Object file handler
                 * @param str data input
                 * @return status of operation
                 */
                status_t            parse_data(IObjHandler *handler, const LSPString *str);

                /**
                 * Parse data from character sequence
                 * @param handler Wavefront Object file handler
                 * @param seq input sequence
                 * @param flags wrap flags
                 * @return status of operation
                 */
                status_t            parse_data(IObjHandler *handler, io::IInSequence *seq, size_t flags = WRAP_NONE);
        };
    
    } /* namespace obj */
} /* namespace lsp */

#endif /* LSP_PLUG_IN_FMT_OBJ_PUSHPARSER_H_ */
