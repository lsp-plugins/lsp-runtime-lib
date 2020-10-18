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
#include <lsp-plug.in/io/charset.h>

namespace lsp
{
    namespace io
    {
        PathPattern::PathPattern()
        {
            nFlags      = 0;
            pRoot       = NULL;
        }

        PathPattern::~PathPattern()
        {
            destroy_data(pRoot);
            pRoot   = NULL;
        }

        void PathPattern::destroy_data(cmd_t *cmd)
        {
            if (cmd == NULL)
                return;

            // Recursively delete all sub-nodes
            for (size_t i=0, n=cmd->sChildren.size(); i<n; ++i)
            {
                cmd_t *child = cmd->sChildren.uget(i);
                destroy_data(child);
            }

            delete cmd;
        }

        void PathPattern::next_token(tokenizer_t *it)
        {
            it->nToken      = -1;
        }

        ssize_t PathPattern::get_token(tokenizer_t *it)
        {
            // There is unget token?
            if (it->nToken >= 0)
                return it->nToken;

            const LSPString *mask   = it->pMask;

            // End of input?
            if (it->nPosition >= mask->length())
                return it->nToken       = T_EOF;

            // Analyze first character
            it->nStart              = it->nPosition;
            it->nEnd                = it->nPosition;
            lsp_wchar_t c           = mask->char_at(it->nPosition++);
            bool escape;

            switch (c)
            {
                case '&': // T_AND
                    return it->nToken = T_AND;
                case '|': // T_OR
                    return it->nToken = T_OR;
                case '!': // T_NOT, TT_IGROUP_START
                    if (it->nPosition >= mask->length())
                        return it->nToken = T_NOT;
                    if (mask->char_at(it->nPosition) != '(')
                        return it->nToken = T_NOT;

                    ++it->nPosition;
                    return it->nToken = T_IGROUP_START;
                case ')': // T_GROUP_END
                    return it->nToken = T_GROUP_END;

                case '(': // T_GROUP_START, T_IGROUP_START
                    return it->nToken = T_GROUP_START;

                case '*': // T_ANY, T_ANYPATH
                    if ((it->nPosition + 2) > mask->length())
                        return it->nToken = T_ANY;
                    if (mask->char_at(it->nPosition) != '*')
                        return it->nToken = T_ANY;
                    c = mask->char_at(it->nPosition+1);
                    if ((c != '\\') && (c != '/'))
                        return it->nToken = T_ANY;

                    it->nPosition += 2;
                    return it->nToken = T_ANYPATH;

                default: // T_TEXT
                    it->nToken      = T_TEXT;
                    escape          = c == '`';
                    break;
            }

            // Parse T_TEXT
            for ( ; it->nPosition < mask->length(); ++it->nPosition )
            {
                // Lookup next character
                c   = mask->char_at(it->nPosition);

                switch (c)
                {
                    // Special symbols
                    case '*': case '(': case ')': case '|':
                    case '&': case '!':
                        if (!escape)
                        {
                            it->nEnd    = it->nPosition;
                            return it->nToken;
                        }
                        escape = false;
                        break;

                    case '`':
                        escape  = !escape;
                        break;

                    default:
                        escape  = false;
                        break;
                }
            }

            // Escape character pending?
            it->nEnd    = it->nPosition;

            return it->nToken;
        }

        status_t PathPattern::merge_step(cmd_t **out, cmd_t *next, command_t type)
        {
            if (*out == NULL)
            {
                cmd_t *tmp      = new cmd_t();
                tmp->nCommand   = type;
                tmp->nStart     = 0;
                tmp->nEnd       = 0;
                tmp->bInverse   = false;
                *out            = tmp;
            }
            if (next == NULL)
                return STATUS_OK;

            return (*out)->sChildren.add(next) ? STATUS_OK : STATUS_NO_MEM;
        }

        status_t PathPattern::merge_simple(cmd_t **out, command_t type, command_t cmd, size_t start, size_t end)
        {
            cmd_t *tmp      = new cmd_t();
            tmp->nCommand   = cmd;
            tmp->nStart     = start;
            tmp->nEnd       = end;
            tmp->bInverse   = false;

            status_t res = merge_step(out, tmp, type);
            if (res != STATUS_OK)
                destroy_data(tmp);
            return res;
        }

        status_t PathPattern::merge_last(cmd_t **dst, cmd_t *out, cmd_t *next, ssize_t tok)
        {
            // Analyze last token
            if (tok < 0)
            {
                destroy_data(next);
                destroy_data(out);
                return -tok;
            }
            else if (out == NULL)
            {
                *dst                = next;
                return STATUS_OK;
            }
            else if (!out->sChildren.add(next))
            {
                destroy_data(out);
                destroy_data(next);
                return STATUS_NO_MEM;
            }

            // Return the output expression
            *dst                = out;

            return STATUS_OK;
        }

        status_t PathPattern::parse_sequence(cmd_t **dst, tokenizer_t *it)
        {
            cmd_t *out = NULL, *next = NULL;
            status_t res;

            while (true)
            {
                ssize_t tok     = get_token(it);

                switch (tok)
                {
                    case T_GROUP_START:
                    case T_IGROUP_START:
                        next_token(it);
                        if ((res = parse_or(&next, it)) != STATUS_OK)
                            break;
                        next->bInverse  = (tok == T_IGROUP_START);
                        if ((res = merge_step(&out, next, CMD_SEQUENCE)) != STATUS_OK)
                        {
                            destroy_data(next);
                            break;
                        }

                        tok = get_token(it);
                        if (tok == T_EOF)
                            return -STATUS_EOF;
                        else if (tok != T_GROUP_END)
                            return -STATUS_BAD_FORMAT;
                        next_token(it);
                        break;

                    case T_TEXT:
                        next_token(it);
                        res = merge_simple(&out, CMD_SEQUENCE, CMD_PATTERN, it->nStart, it->nEnd);
                        break;

                    case T_ANY:
                        next_token(it);
                        // Premature optimization: Reduce sequence of * to one element
                        next = (out != NULL) ? out->sChildren.last() : NULL;
                        if ((next != NULL) && (next->nCommand == CMD_ANY))
                            break;
                        res = merge_simple(&out, CMD_SEQUENCE, CMD_ANY, 0, 0);
                        break;

                    case T_ANYPATH:
                        next_token(it);
                        next = (out != NULL) ? out->sChildren.last() : NULL;
                        // Premature optimization: Reduce sequence of **/ to one element
                        if ((next != NULL) && (next->nCommand == CMD_ANYPATH))
                            break;
                        res = merge_simple(&out, CMD_SEQUENCE, CMD_ANYPATH, 0, 0);
                        break;

                    default:
                        res = merge_step(&out, NULL, CMD_SEQUENCE);
                        if (res != STATUS_OK)
                            break;

                        if (out->sChildren.size() == 1)
                        {
                            // Premature optimization: treat sequence of one element as this element
                            *dst = out->sChildren.uget(0);
                            out->sChildren.clear();
                            destroy_data(out);
                        }
                        else
                        {
                            if (out->sChildren.is_empty())
                            {
                                // Premature optimization: Treat empty sequence as empty text
                                out->nCommand   = CMD_PATTERN;
                                out->nStart     = 0;
                                out->nEnd       = 0;
                            }

                            *dst = out;
                        }

                        return res;
                }

                if (res != STATUS_OK)
                {
                    destroy_data(out);
                    return res;
                }
            }
        }

        status_t PathPattern::parse_not(cmd_t **dst, tokenizer_t *it)
        {
            bool inverse    = false;

            // Premature optimization: treat multiple ! as valid inverse
            ssize_t tok     = get_token(it);
            while (tok == T_NOT)
            {
                next_token(it);
                inverse         = !inverse;
                tok             = get_token(it);
            }

            status_t res = parse_sequence(dst, it);
            if (res == STATUS_OK)
                (*dst)->bInverse    = inverse;

            return res;
        }

        status_t PathPattern::parse_and(cmd_t **dst, tokenizer_t *it)
        {
            cmd_t *out = NULL, *next = NULL;

            // Parse sub-expression
            status_t res    = parse_not(&next, it);
            if (res != STATUS_OK)
                return res;

            // Get token (and move to next)
            ssize_t tok     = get_token(it);
            while (tok == T_AND)
            {
                // Commit token
                next_token(it);

                // Merge command and parse next sub-expression
                if ((res = merge_step(&out, next, CMD_AND)) == STATUS_OK)
                    res = parse_not(&next, it);

                // Parse command
                if (res != STATUS_OK)
                {
                    destroy_data(out);
                    destroy_data(next);
                    return res;
                }

                // Get next token
                tok                 = get_token(it);
            }

            return merge_last(dst, out, next, tok);
        }

        status_t PathPattern::parse_or(cmd_t **dst, tokenizer_t *it)
        {
            cmd_t *out = NULL, *next = NULL;

            // Parse sub-expression
            status_t res    = parse_and(&next, it);
            if (res != STATUS_OK)
                return res;

            // Get token (and move to next)
            ssize_t tok     = get_token(it);
            while (tok == T_OR)
            {
                // Commit token
                next_token(it);

                // Merge command and parse next sub-expression
                if ((res = merge_step(&out, next, CMD_OR)) == STATUS_OK)
                    res = parse_and(&next, it);

                // Parse command
                if (res != STATUS_OK)
                {
                    destroy_data(out);
                    destroy_data(next);
                    return res;
                }

                // Get next token
                tok                 = get_token(it);
            }

            return merge_last(dst, out, next, tok);
        }

        status_t PathPattern::parse(const LSPString *pattern, size_t flags)
        {
            tokenizer_t it;
            PathPattern tmp;

            // Initialize temporary pattern
            if (!tmp.sMask.set(pattern))
                return STATUS_NO_MEM;
            tmp.nFlags = flags;

            // Initialize tokenizer
            it.nToken       = -1;
            it.pMask        = &tmp.sMask;
            it.nPosition    = 0;
            it.nStart       = 0;
            it.nEnd         = 0;

            // Parse expression
            status_t res = parse_or(&tmp.pRoot, &it);
            if (res == STATUS_OK)
            {
                ssize_t tok = get_token(&it);
                if (tok < 0)
                    return -tok;
                else if (tok != T_EOF)
                    return STATUS_BAD_FORMAT;

                tmp.swap(this); // Apply new value on success
            }

            return res;
        }

        bool PathPattern::pattern_matcher_seek(matcher_t *m, const pos_t *pos)
        {
            text_matcher_t *sm = static_cast<text_matcher_t *>(m);

            sm->pos.start   = pos->start;
            sm->pos.end     = pos->end;
            sm->calls       = 0;
            return true;
        }

        bool PathPattern::pattern_matcher_match(matcher_t *m, pos_t *pos)
        {
            text_matcher_t *sm = static_cast<text_matcher_t *>(m);
            const cmd_t *cmd = sm->cmd;

            // Simple matcher can be called only once
            if (sm->calls > 0)
                return false;
            ++sm->calls;

            size_t len = cmd->nEnd - cmd->nStart;
            bool match;


//
//            switch (cmd->nCommand)
//            {
//                case CMD_TEXT:
//                {
//                    const lsp_wchar_t *s1 = sm->buf->characters() + cmd->nStart;
//                    const lsp_wchar_t *s2 = sm->str->characters() + sm->pos.start;
//                    match = ((sm->flags & CASE_SENSITIVE) ? wchar_cmp(s1, s2, len) : wchar_casecmp(s1, s2, len)) == 0;
//                    break;
//                }
//                case CMD_SPLIT:
//                {
//                    lsp_wchar_t c = sm->buf->char_at(sm->pos.start);
//                    match = (c == '/') || (c == '\\');
//                    break;
//                }
//                case CMD_WILDCARD:
//                {
//                    size_t len = cmd->nEnd - cmd->nStart;
//                    const lsp_wchar_t *s = sm->str->characters() + sm->pos.start;
//                    for ( ; len > 0; --len)
//                    {
//                        if ((*s == '/') || ((*s) == '\\'))
//                            return false;
//                    }
//                    match = true;
//                    break;
//                }
//                default:
//                    return false;
//            }

            // Analyze match
            if (cmd->bInverse == match)
                return false;
            pos->start      = (match) ? sm->pos.start + len : sm->pos.start;
            pos->end        = sm->pos.end;

            return true;
        }

        void PathPattern::destroy_matcher(matcher_t *m)
        {
            if (m == NULL)
                return;

            const cmd_t *cmd = m->cmd;
            switch (cmd->nCommand)
            {
                case CMD_SEQUENCE:
                    // TODO
                    break;
                case CMD_AND:
                    // TODO
                    break;
                case CMD_OR:
                    // TODO
                    break;

                case CMD_ANYPATH:
                    // TODO
                    break;

                case CMD_ANY:
                    // TODO
                    break;

                // Simple matchers
                case CMD_PATTERN:
                {
                    text_matcher_t *sm = static_cast<text_matcher_t *>(m);
                    delete sm;
                    break;
                }

                default:
                    break;
            }
        }

        PathPattern::matcher_t *PathPattern::create_root_matcher()
        {
            return NULL;
        }

        bool PathPattern::match_full(const LSPString *path) const
        {
            return false;
//            matcher_t *it = create_root_matcher();
//            it.cmd      = pRoot;
//            it.str      = path;
//            it.buf      = &sBuffer;
//            it.flags    = nFlags & ;
//            it.nStart   = 0;
//            it.nEnd     = path->length();

//            pos_t pos;
//            bool match;
//
//            while (true)
//            {
//                // Is there any next match?
//                if (!next_match(&it, &pos))
//                {
//                    match = false;
//                    break;
//                }
//
//                // There is nothing left?
//                if (pos.nStart == pos.nEnd)
//                {
//                    match = !it.pCmd->bInverse;
//                    break;
//                }
//            }
//
//            return (nFlags & INVERSIVE) ? !match : match;
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

        bool PathPattern::test(const char *path) const
        {
            if (pRoot == NULL)
                return false;

            Path tmp;
            if (tmp.set(path) != STATUS_OK)
                return false;

            if (!(nFlags & FULL_PATH))
            {
                if (tmp.remove_base() != STATUS_OK)
                    return false;
            }

            return match_full(tmp.as_string());
        }

        bool PathPattern::test(const LSPString *path) const
        {
            if (pRoot == NULL)
                return false;

            Path tmp;
            if (tmp.set(path) != STATUS_OK)
                return false;

            if (!(nFlags & FULL_PATH))
            {
                if (tmp.remove_base() != STATUS_OK)
                    return false;
            }

            return match_full(tmp.as_string());
        }

        bool PathPattern::test(const Path *path) const
        {
            if (pRoot == NULL)
                return false;

            if (nFlags & FULL_PATH)
                return match_full(path->as_string());

            Path tmp;
            if (tmp.get_last(&tmp) != STATUS_OK)
                return false;

            return match_full(tmp.as_string());
        }

        void PathPattern::swap(PathPattern *dst)
        {
            sMask.swap(dst->sMask);
            lsp::swap(pRoot, dst->pRoot);
            lsp::swap(nFlags, dst->nFlags);
        }
    }
}
