/*
 * Copyright (C) 2023 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2023 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-runtime-lib
 * Created on: 29 янв. 2023 г.
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

#ifndef LSP_PLUG_IN_FMT_SFZ_PULLPARSER_H_
#define LSP_PLUG_IN_FMT_SFZ_PULLPARSER_H_

#include <lsp-plug.in/runtime/version.h>

#include <lsp-plug.in/io/IInSequence.h>
#include <lsp-plug.in/io/IInStream.h>
#include <lsp-plug.in/io/Path.h>

#include <lsp-plug.in/fmt/sfz/types.h>

namespace lsp
{
    namespace sfz
    {
        /**
         * SFZ file format pull parser
         */
        class LSP_RUNTIME_LIB_PUBLIC PullParser
        {
            private:
                io::IInSequence    *pIn;
                size_t              nWFlags;
                event_t             sCurrent;
                LSPString           sUnget;             // Buffer for 'unget' operations
                size_t              nUnget;             // Read offset relative to the beginning of the 'unget' buffer

            protected:
                static bool         is_identifier(lsp_wchar_t ch, bool first);
                static bool         is_space(lsp_wchar_t ch);
                static void         trim_right(LSPString *value);

            protected:
                lsp_swchar_t        get_char();
                status_t            expect_string(const char *text);
                status_t            expect_char(lsp_swchar_t expected);
                status_t            read_opcode(lsp_wchar_t ch, event_t *ev);
                status_t            read_opcode_name(lsp_swchar_t ch, LSPString *name);
                status_t            read_opcode_value(LSPString *value);
                status_t            read_sample_file_name(LSPString *value);
                status_t            read_variable_name(LSPString *value);
                status_t            read_variable_value(LSPString *value);

                status_t            read_next_event(event_t *ev);
                status_t            read_header(event_t *ev);
                status_t            read_comment(event_t *ev);
                status_t            read_define(event_t *ev);
                status_t            read_include(event_t *ev);
                status_t            read_preprocessor(event_t *ev);
                status_t            set_error(status_t code);

            public:
                explicit PullParser();
                ~PullParser();

            public:
                /**
                 * Open parser
                 * @param path UTF-8 path to the file
                 * @param version JSON version
                 * @param charset character set, UTF-8 if not specified
                 * @return status of operation
                 */
                status_t    open(const char *path, const char *charset = NULL);

                /**
                 * Open parser
                 * @param path string representation of path to the file
                 * @param version JSON version
                 * @param charset character set, UTF-8 if not specified
                 * @return status of operation
                 */
                status_t    open(const LSPString *path, const char *charset = NULL);

                /**
                 * Open parser
                 * @param path path to the file
                 * @param version JSON version
                 * @param charset character set, UTF-8 if not specified
                 * @return status of operation
                 */
                status_t    open(const io::Path *path, const char *charset = NULL);

                /**
                 * Wrap string with parser
                 * @param str string to wrap
                 * @param version JSON version
                 * @param charset character set, UTF-8 if not specified
                 * @return status of operation
                 */
                status_t    wrap(const char *str, const char *charset = NULL);

                /**
                 * Wrap string with parser
                 * @param str string to wrap
                 * @param version JSON version, UTF-8 if not specified
                 * @return status of operation
                 */
                status_t    wrap(const LSPString *str);

                /**
                 * Wrap input sequence with parser
                 * @param seq sequence to use for reads
                 * @param version JSON version
                 * @return status of operation
                 */
                status_t    wrap(io::IInSequence *seq, size_t flags = WRAP_NONE);

                /**
                 * Wrap input stream with parser
                 * @param is input stream
                 * @param version JSON version
                 * @param flags wrap flags
                 * @param charset character set, UTF-8 if not specified
                 * @return status of operation
                 */
                status_t    wrap(io::IInStream *is, size_t flags = WRAP_NONE, const char *charset = NULL);

                /**
                 * Close parser
                 * @return status of operation
                 */
                status_t    close();

            public:
                /**
                 * Get current event
                 * @return current event
                 */
                const event_t  *current() const;

                /**
                 * Get next event
                 * @param event pointer to store the event data, can be NULL
                 * @return status of operation
                 */
                status_t        next(event_t *event = NULL);
        };

    } /* namespace sfz */
} /* namespace lsp */



#endif /* LSP_PLUG_IN_FMT_SFZ_PULLPARSER_H_ */
