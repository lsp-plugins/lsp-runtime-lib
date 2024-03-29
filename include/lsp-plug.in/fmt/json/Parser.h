/*
 * Copyright (C) 2023 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2023 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-runtime-lib
 * Created on: 16 окт. 2019 г.
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

#ifndef LSP_PLUG_IN_FMT_JSON_PARSER_H_
#define LSP_PLUG_IN_FMT_JSON_PARSER_H_

#include <lsp-plug.in/runtime/version.h>
#include <lsp-plug.in/common/types.h>
#include <lsp-plug.in/io/IInStream.h>
#include <lsp-plug.in/io/Path.h>
#include <lsp-plug.in/fmt/json/token.h>
#include <lsp-plug.in/fmt/json/Tokenizer.h>
#include <lsp-plug.in/lltl/darray.h>

namespace lsp
{
    namespace json
    {
        
        class Parser
        {
            protected:
                enum pmode_t
                {
                    READ_ROOT,
                    READ_ARRAY,
                    READ_OBJECT
                };

                enum parse_flags_t
                {
                    PF_COMMA        = 1 << 0,
                    PF_COLON        = 1 << 1,
                    PF_PROPERTY     = 1 << 2,
                    PF_VALUE        = 1 << 3,

                    PF_ARRAY_ALL    = PF_COMMA | PF_VALUE,
                    PF_OBJECT_ALL   = PF_COMMA | PF_COLON | PF_PROPERTY | PF_VALUE
                };

                typedef struct state_t
                {
                    pmode_t                 mode;
                    size_t                  flags;
                } state_t;

            protected:
                Tokenizer              *pTokenizer;
                io::IInSequence        *pSequence;
                size_t                  nWFlags;
                json_version_t          enVersion;
                state_t                 sState;
                event_t                 sCurrent;
                lltl::darray<state_t>   sStack;

            protected:
                status_t            read_root();
                status_t            read_array();
                status_t            read_object();
                status_t            read_primitive(token_t tok);

                inline status_t     push_state(pmode_t state);
                inline status_t     pop_state();

                inline token_t      lookup_token();

            public:
                explicit Parser();
                Parser(const Parser &) = delete;
                Parser(Parser &&) = delete;
                virtual ~Parser();

                Parser & operator = (const Parser &) = delete;
                Parser & operator = (Parser &&) = delete;

            public:
                /**
                 * Open parser
                 * @param path UTF-8 path to the file
                 * @param version JSON version
                 * @param charset character set
                 * @return status of operation
                 */
                status_t    open(const char *path, json_version_t version, const char *charset = NULL);

                /**
                 * Open parser
                 * @param path string representation of path to the file
                 * @param version JSON version
                 * @param charset character set
                 * @return status of operation
                 */
                status_t    open(const LSPString *path, json_version_t version, const char *charset = NULL);

                /**
                 * Open parser
                 * @param path path to the file
                 * @param version JSON version
                 * @param charset character set
                 * @return status of operation
                 */
                status_t    open(const io::Path *path, json_version_t version, const char *charset = NULL);

                /**
                 * Wrap string with parser
                 * @param str string to wrap
                 * @param version JSON version
                 * @param charset character set
                 * @return status of operation
                 */
                status_t    wrap(const char *str, json_version_t version, const char *charset = NULL);

                /**
                 * Wrap string with parser
                 * @param str string to wrap
                 * @param version JSON version
                 * @return status of operation
                 */
                status_t    wrap(const LSPString *str, json_version_t version);

                /**
                 * Wrap input sequence with parser
                 * @param seq sequence to use for reads
                 * @param version JSON version
                 * @return status of operation
                 */
                status_t    wrap(io::IInSequence *seq, json_version_t version, size_t flags = WRAP_NONE);

                /**
                 * Wrap input stream with parser
                 * @param is input stream
                 * @param version JSON version
                 * @param flags wrap flags
                 * @param charset character set
                 * @return status of operation
                 */
                status_t    wrap(io::IInStream *is, json_version_t version, size_t flags = WRAP_NONE, const char *charset = NULL);

                /**
                 * Close parser
                 * @return status of operation
                 */
                status_t    close();

            public:
                /**
                 * Read next event
                 * @param ev event to read
                 * @return status of operation
                 */
                status_t    read_next(event_t *ev);

                /**
                 * Read next event and return it's type. The event
                 * will be accessible by get_current() call
                 *
                 * @param type event type
                 * @return status of operation
                 */
                status_t    read_next_type(event_type_t *type);

                /**
                 * Read string value
                 * @param dst pointer to string to store the value
                 * @return status of operation, STATUS_NULL if value is null
                 */
                status_t    read_string(LSPString *dst);

                /**
                 * Read double value
                 * @param dst pointer to store the value
                 * @return status of operation, STATUS_NULL if value is null
                 */
                status_t    read_double(double *dst);

                /**
                 * Read integer value
                 * @param dst pointer to store the value
                 * @return status of operation, STATUS_NULL if value is null
                 */
                status_t    read_int(ssize_t *dst);

                /**
                 * Read boolean value
                 * @param dst pointer to store the value
                 * @return status of operation, STATUS_NULL if value is null
                 */
                status_t    read_bool(bool *dst);

                /**
                 * Get current event
                 * @param ev pointer to save the event
                 * @return status of operation
                 */
                status_t    get_current(event_t *ev);

                /**
                 * Get current string value
                 * @param dst pointer to string to store the value
                 * @return status of operation, STATUS_NULL if value is null
                 */
                status_t    get_string(LSPString *dst);

                /**
                 * Get current double value
                 * @param dst pointer to store the value
                 * @return status of operation, STATUS_NULL if value is null
                 */
                status_t    get_double(double *dst);

                /**
                 * Get current integer value
                 * @param dst pointer to store the value
                 * @return status of operation, STATUS_NULL if value is null
                 */
                status_t    get_int(ssize_t *dst);

                /**
                 * Get current boolean value
                 * @param dst pointer to store the value
                 * @return status of operation, STATUS_NULL if value is null
                 */
                status_t    get_bool(bool *dst);

                /**
                 * Skip nested data structures after current event
                 * @return status of operation
                 */
                status_t    skip_next();

                /**
                 * Skip nested data structures associated with current event.
                 * If current event is JE_OBJECT_START
                 * @return status of operation
                 */
                status_t    skip_current();
        };
    
    } /* namespace json */
} /* namespace lsp */

#endif /* LSP_PLUG_IN_FMT_JSON_PARSER_H_ */
