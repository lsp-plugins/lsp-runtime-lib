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

#include <lsp-plug.in/io/PathPattern.h>

namespace lsp
{
    namespace io
    {
        PathPattern::PathPattern()
        {
            nFlags      = 0;
        }

        PathPattern::~PathPattern()
        {
        }

        status_t PathPattern::parse(const LSPString *pattern, size_t flags)
        {
            return STATUS_NOT_IMPLEMENTED;
        }

        PathPattern::smask_t *PathPattern::parse_simple(lltl::darray<smask_t> *dst, biter_t *bi)
        {
            return NULL;
        }

        bool PathPattern::check_simple_case(const lsp_wchar_t *head, const lsp_wchar_t *tail, const lsp_wchar_t *shead, const lsp_wchar_t *stail)
        {
            return false;
        }

        bool PathPattern::check_simple_nocase(const lsp_wchar_t *head, const lsp_wchar_t *tail, const lsp_wchar_t *shead, const lsp_wchar_t *stail)
        {
            return false;
        }

        bool PathPattern::check_mask(smask_t *mask, const lsp_wchar_t *s, size_t len)
        {
            return false;
        }

        bool PathPattern::check_match(const LSPString *path)
        {
            return false;
        }

        status_t PathPattern::set(const char *pattern, size_t flags)
        {
            Path tmp;
            status_t res = tmp.set(pattern);
            if (res == STATUS_OK)
                res     = parse(tmp.as_string(), flags);
            return res;
        }

        status_t PathPattern::set(const LSPString *pattern, size_t flags)
        {
            Path tmp;
            status_t res = tmp.set(pattern);
            if (res == STATUS_OK)
                res     = parse(tmp.as_string(), flags);
            return res;
        }

        size_t PathPattern::set_flags(size_t flags)
        {
            size_t old  = nFlags;
            nFlags      = flags & (INVERSIVE | CASE_SENSITIVE | FULL_PATH);
            return old;
        }

        bool PathPattern::test(const char *text)
        {
            Path tmp;
            return (tmp.set(text)) ? check_match(tmp.as_string()) : false;
        }

        bool PathPattern::test(const LSPString *text)
        {
            Path tmp;
            return (tmp.set(text)) ? check_match(tmp.as_string()) : false;
        }

        void PathPattern::swap(PathPattern *dst)
        {
            sMask.swap(dst->sMask);
            vMasks.swap(dst->vMasks);
            lsp::swap(nFlags, dst->nFlags);
        }
    }
}
