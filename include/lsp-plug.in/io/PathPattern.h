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
#include <lsp-plug.in/lltl/darray.h>

namespace lsp
{
    namespace io
    {
        /**
         * The simple pattern for the file name
         */
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
                typedef struct smask_t
                {
                    const lsp_wchar_t      *pHead;
                    const lsp_wchar_t      *pTail;
                    bool                    bInvert;
                } smask_t;

                typedef struct biter_t
                {
                    lsp_wchar_t            *pHead;
                    lsp_wchar_t            *pTail;
                    bool                    bInvert;
                } biter_t;

            protected:
                LSPString                   sBuffer;
                LSPString                   sMask;
                lltl::darray<smask_t>       vMasks;
                size_t                      nFlags;

            private:
                PathPattern & operator = (const PathPattern &);

            protected:
                status_t                    parse(const LSPString *pattern, size_t flags = NONE);
                static smask_t             *parse_simple(lltl::darray<smask_t> *dst, biter_t *bi);
                static bool                 check_simple_case(const lsp_wchar_t *head, const lsp_wchar_t *tail, const lsp_wchar_t *shead, const lsp_wchar_t *stail);
                static bool                 check_simple_nocase(const lsp_wchar_t *head, const lsp_wchar_t *tail, const lsp_wchar_t *shead, const lsp_wchar_t *stail);
                bool                        check_mask(smask_t *mask, const lsp_wchar_t *s, size_t len);

                bool                        check_match(const LSPString *path);

            public:
                explicit PathPattern();
                ~PathPattern();

            public:
                status_t                    set(const PathPattern *src)                                 { return parse(&src->sMask, src->nFlags);   }
                status_t                    set(const Path *pattern, size_t flags = NONE)               { return parse(pattern->as_string(), flags);}
                status_t                    set(const LSPString *pattern, size_t flags = NONE);
                status_t                    set(const char *pattern, size_t flags = NONE);

                inline const char          *pattern() const                                             { return sMask.get_utf8();                  }
                inline status_t             pattern(LSPString *mask) const                              { return (mask != NULL) ? mask->set(&sMask) : STATUS_BAD_ARGUMENTS; }
                inline status_t             pattern(Path *mask) const                                   { return (mask != NULL) ? mask->set(&sMask) : STATUS_BAD_ARGUMENTS; }

                inline size_t               flags() const                                               { return nFlags;                            }
                size_t                      set_flags(size_t flags);

                inline status_t             set_pattern(const PathPattern *src)                         { return set(&src->sMask, nFlags);          }
                inline status_t             set_pattern(const LSPString *pattern)                       { return set(pattern, nFlags);              }
                inline status_t             set_pattern(const char *pattern)                            { return set(pattern, nFlags);              }

                bool                        test(const char *text);
                bool                        test(const LSPString *text);
                inline bool                 test(const Path *path)                                      { return check_match(path->as_string());    }

                void                        swap(PathPattern *dst);
                inline void                 swap(PathPattern &dst)                                      { swap(&dst);                               }
        };
    }
} /* namespace lsp */

#endif /* LSP_PLUG_IN_IO_PATHPATTERN_H_ */
