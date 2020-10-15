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

        bool PathPattern::check_match(const LSPString *path)
        {
            return false;
        }

        status_t PathPattern::set(const char *pattern, size_t flags)
        {
            LSPString tmp;
            return (tmp.set_utf8(pattern)) ? parse(&tmp, flags) : STATUS_NO_MEM;
        }

        size_t PathPattern::set_flags(size_t flags)
        {
            size_t old  = nFlags;
            nFlags      = flags & (INVERSIVE | CASE_SENSITIVE | FULL_PATH);
            return old;
        }

        bool PathPattern::test(const char *path)
        {
            Path tmp;
            if (tmp.set(path) != STATUS_OK)
                return false;

            if (!(nFlags & FULL_PATH))
            {
                if (tmp.remove_base() != STATUS_OK)
                    return false;
            }

            return (nFlags & FULL_PATH) ? check_match(tmp.as_string()) : false;
        }

        bool PathPattern::test(const LSPString *path)
        {
            Path tmp;
            if (tmp.set(path) != STATUS_OK)
                return false;

            if (!(nFlags & FULL_PATH))
            {
                if (tmp.remove_base() != STATUS_OK)
                    return false;
            }

            return (nFlags & FULL_PATH) ? check_match(tmp.as_string()) : false;
        }

        bool PathPattern::test(const Path *path)
        {
            if (!(nFlags & FULL_PATH))
                return check_match(path->as_string());

            Path tmp;
            if (tmp.get_last(&tmp) != STATUS_OK)
                return false;

            return check_match(tmp.as_string());
        }

        void PathPattern::swap(PathPattern *dst)
        {
            sBuffer.swap(dst->sBuffer);
            sMask.swap(dst->sMask);
            lsp::swap(sRoot, dst->sRoot);
            lsp::swap(nFlags, dst->nFlags);
        }
    }
}
