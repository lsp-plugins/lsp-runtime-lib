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

        ssize_t PathPattern::get_token(tokenizer_t *it)
        {
            // There is unget token?
            if (it->nToken >= 0)
                return it->nToken;

            // End of input?
            if (it->nPosition >= sMask.length())
                return -STATUS_EOF;

            // Analyze first character
            bool escape     = false;
            lsp_wchar_t c   = sMask.char_at(it->nPosition++);

            switch (c)
            {
                case '?': // T_WILDCARD
                    return it->nToken = T_WILDCARD;
                case '&': // T_AND
                    return it->nToken = T_AND;
                case '|': // T_OR
                    return it->nToken = T_OR;
                case ')': // T_GROUP_END
                    return it->nToken = T_GROUP_END;


                case '\\':
                case '/': // T_SPLIT
                    return it->nToken = T_SPLIT;
                case '(': // T_GROUP_START, T_IGROUP_START
                    if (it->nPosition >= sMask.length())
                        return it->nToken = T_GROUP_START;
                    c = sMask.char_at(it->nPosition);
                    if (c != '!')
                        return it->nToken = T_GROUP_START;
                    ++it->nPosition;
                    return it->nToken = T_IGROUP_START;

                case '*': // T_ANY, T_ANYPATH
                    if ((it->nPosition + 2) > sMask.length())
                        return it->nToken = T_ANY;
                    if (sMask.char_at(it->nPosition) != '*')
                        return it->nToken = T_ANY;
                    c = sMask.char_at(it->nPosition+1);
                    if ((c != '\\') && (c != '/'))
                        return it->nToken = T_ANY;

                    it->nPosition += 2;
                    return it->nToken = T_ANYPATH;

                case '`': // T_TEXT starting from escape
                    escape          = true;
                    it->nToken      = T_TEXT;
                    it->nStart      = sBuffer.length();
                    break;

                default: // T_TEXT
                    it->nToken      = T_TEXT;
                    it->nStart      = sBuffer.length();
                    break;
            }

            // Parse T_TEXT
            for ( ; it->nPosition < sMask.length(); ++it->nPosition)
            {
                // Lookup next character
                lsp_wchar_t c   = sMask.char_at(it->nPosition);

                switch (c)
                {
                    // Special symbols
                    case '*': case '/': case '\\': case '(':
                    case ')': case '|': case '&':
                        if (!escape)
                        {
                            it->nLength     = sBuffer.length() - it->nStart;
                            return it->nToken;
                        }

                        if (!sBuffer.append(c))
                            return -STATUS_NO_MEM;
                        escape =  false;
                        break;

                    case '`':
                        if (escape)
                        {
                            if (!sBuffer.append(c))
                                return -STATUS_NO_MEM;
                        }
                        escape = !escape;
                        break;

                    default:
                        if (escape)
                        {
                            if (!sBuffer.append('`'))
                                return -STATUS_NO_MEM;
                            escape = false;
                        }
                        if (!sBuffer.append(c))
                            return -STATUS_NO_MEM;
                        break;
                }
            }

            // Escape character pending?
            if (escape)
            {
                if (!sBuffer.append('`'))
                    return -STATUS_NO_MEM;
            }
            return it->nToken;
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
