/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-runtime-lib
 * Created on: 15 окт. 2020 г.
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

#ifndef LSP_PLUG_IN_IO_PATHPATTERN_H_
#define LSP_PLUG_IN_IO_PATHPATTERN_H_

#include <lsp-plug.in/runtime/version.h>
#include <lsp-plug.in/runtime/LSPString.h>
#include <lsp-plug.in/io/Path.h>
#include <lsp-plug.in/common/status.h>
#include <lsp-plug.in/lltl/parray.h>

namespace lsp
{
    namespace io
    {
        //
        // The pattern for the file name.
        //
        // Syntax:
        //   *             - Any character sequence except '/' or '\'
        //   ?             - Any character except '/' or '\'
        //   / or \        - Path separator
        //   `/            - Escaped character '/' (Available for escaping: '*', '?', '`', '/', '\', '&', '|', '(', ')' )
        //   name.ext      - Strict match of characters to 'name.ext'
        //   ... & ...     - Conjunction of two conditions
        //   ... | ...     - Disjunction of two conditions
        //   ! ...         - Negotation of match
        //   **/ or **\     - Any path
        //   ( ... )       - Pattern group
        //
        // Example:
        //   **/((*.c|*.h)&(test-*)) - matches any source/header C files for any subdirectory starting with 'test-' prefix
        class PathPattern
        {
            public:
                enum flags
                {
                    INVERSIVE       = 1 << 0,
                    CASE_SENSITIVE  = 1 << 1,
                    FULL_PATH       = 1 << 2,

                    NONE            = 0
                };

            protected:
                enum command_t
                {
                    CMD_SEQUENCE,       // cmd1 cmd2 ...
                    CMD_AND,            // ... & ...
                    CMD_OR,             // ... | ...
                    CMD_PATTERN,           // ... text ...
                    CMD_ANY,            // ... * ...
                    CMD_ANYPATH,        // ... **/ ...
                };

                enum token_type_t
                {
                    T_GROUP_START,      // (
                    T_IGROUP_START,     // !(
                    T_GROUP_END,        // )
                    T_OR,               // |
                    T_AND,              // &
                    T_NOT,              // !
                    T_TEXT,             // text
                    T_ANY,              // *
                    T_ANYPATH,          // **/
                    T_EOF               // <eof>
                };

                typedef struct cmd_t
                {
                    command_t               nCommand;   // Command
                    size_t                  nStart;     // Start of character sequence
                    size_t                  nLength;    // Length of character sequence
                    size_t                  nChars;     // Actual length in characters (without escaping)
                    bool                    bInverse;   // Inverse condition flag
                    lltl::parray<cmd_t>     sChildren;
                } cmd_t;

                typedef struct tokenizer_t
                {
                    ssize_t                 nToken;

                    const LSPString        *pMask;
                    size_t                  nPosition;
                    size_t                  nStart;     // Start position of token data
                    size_t                  nLength;    // Length of character sequence
                    size_t                  nChars;     // Actual length in characters (without escaping)
                } token_t;

            protected:
                struct matcher_t;

                typedef struct pos_t
                {
                    size_t                  start;
                    size_t                  count;
                } pos_t;

                typedef bool (*match_seek_t) (matcher_t *m, const pos_t *it);
                typedef bool (*match_match_t)(matcher_t *m, pos_t *it);

                typedef struct matcher_t
                {
                    match_seek_t            seek;
                    match_match_t           match;

                    const cmd_t            *cmd;        // Command
                    const LSPString        *mask;       // Source data
                    const LSPString        *str;        // Destination string to match
                    pos_t                   pos;        // Position in destination string
                    size_t                  flags;      // Flags
                } matcher_t;

                typedef struct text_matcher_t: public matcher_t
                {
                    size_t                  calls;      // Number of calls
                } text_matcher_t;

            protected:
                LSPString                   sMask;
                cmd_t                      *pRoot;
                size_t                      nFlags;

            private:
                PathPattern & operator = (const PathPattern &);

            protected:
                status_t                    parse(const LSPString *pattern, size_t flags = NONE);
                bool                        match_full(const LSPString *path) const;

                static ssize_t              get_token(tokenizer_t *it);
                static inline void          next_token(tokenizer_t *it);
                static status_t             parse_not(cmd_t **dst, tokenizer_t *it);
                static status_t             parse_and(cmd_t **dst, tokenizer_t *it);
                static status_t             parse_or(cmd_t **dst, tokenizer_t *it);
                static status_t             parse_sequence(cmd_t **dst, tokenizer_t *it);
                static void                 destroy_data(cmd_t *cmd);
                static status_t             merge_simple(cmd_t **out, command_t type, command_t cmd, tokenizer_t *it);
                static status_t             merge_step(cmd_t **out, cmd_t *next, command_t type);
                static status_t             merge_last(cmd_t **dst, cmd_t *out, cmd_t *next, ssize_t tok);

                static matcher_t           *create_matcher(const cmd_t *cmd, const matcher_t *src, const pos_t *pos);
                static void                 destroy_matcher(matcher_t *match);
                matcher_t                  *create_root_matcher();

                static bool                 pattern_matcher_seek(matcher_t *m, const pos_t *it);
                static bool                 pattern_matcher_match(matcher_t *m, pos_t *it);

            public:
                explicit PathPattern();
                ~PathPattern();

            public:
                status_t                    set(const PathPattern *src)                                 { return parse(&src->sMask, src->nFlags);   }
                status_t                    set(const Path *pattern, size_t flags = NONE)               { return parse(pattern->as_string(), flags);}
                status_t                    set(const LSPString *pattern, size_t flags = NONE)          { return parse(pattern, flags);             }
                status_t                    set(const char *pattern, size_t flags = NONE);

                inline const char          *pattern() const                                             { return sMask.get_utf8();                  }
                inline status_t             pattern(LSPString *mask) const                              { return (mask != NULL) ? mask->set(&sMask) : STATUS_BAD_ARGUMENTS; }
                inline status_t             pattern(Path *mask) const                                   { return (mask != NULL) ? mask->set(&sMask) : STATUS_BAD_ARGUMENTS; }

                inline size_t               flags() const                                               { return nFlags;                            }
                size_t                      set_flags(size_t flags);

                inline status_t             set_pattern(const PathPattern *src)                         { return set(&src->sMask, nFlags);          }
                inline status_t             set_pattern(const LSPString *pattern)                       { return set(pattern, nFlags);              }
                inline status_t             set_pattern(const char *pattern)                            { return set(pattern, nFlags);              }

                bool                        test(const char *path) const;
                bool                        test(const LSPString *path) const;
                inline bool                 test(const Path *path) const;

                void                        swap(PathPattern *dst);
                inline void                 swap(PathPattern &dst)                                      { swap(&dst);                               }
        };
    }
} /* namespace lsp */

#endif /* LSP_PLUG_IN_IO_PATHPATTERN_H_ */
