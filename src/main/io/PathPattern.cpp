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

#include <wctype.h>

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
            it->nLength             = 0;
            it->nChars              = 0;
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
                    if (!escape)
                        ++it->nChars;
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
                            it->nLength = it->nPosition - it->nStart;
                            return it->nToken;
                        }
                        ++it->nChars;
                        escape = false;
                        break;

                    case '`':
                        if (escape)
                            ++it->nChars;
                        escape  = !escape;
                        break;

                    default:
                        ++it->nChars;
                        escape  = false;
                        break;
                }
            }

            // Escape character pending?
            if (escape)
                ++it->nChars;
            it->nLength     = it->nPosition - it->nStart;

            return it->nToken;
        }

        status_t PathPattern::merge_step(cmd_t **out, cmd_t *next, command_t type)
        {
            if (*out == NULL)
            {
                cmd_t *tmp      = new cmd_t();
                tmp->nCommand   = type;
                tmp->nStart     = 0;
                tmp->nLength    = 0;
                tmp->nChars     = 0;
                tmp->bInverse   = false;
                *out            = tmp;
            }
            if (next == NULL)
                return STATUS_OK;

            return (*out)->sChildren.add(next) ? STATUS_OK : STATUS_NO_MEM;
        }

        status_t PathPattern::merge_simple(cmd_t **out, command_t type, command_t cmd, tokenizer_t *it)
        {
            cmd_t *tmp      = new cmd_t();
            tmp->nCommand   = cmd;
            tmp->nStart     = it->nStart;
            tmp->nLength    = it->nLength;
            tmp->nChars     = it->nChars;
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
                        res = merge_simple(&out, CMD_SEQUENCE, CMD_PATTERN, it);
                        break;

                    case T_ANY:
                        next_token(it);
                        // Premature optimization: Reduce sequence of * to one element
                        next = (out != NULL) ? out->sChildren.last() : NULL;
                        if ((next != NULL) && (next->nCommand == CMD_ANY))
                            break;
                        res = merge_simple(&out, CMD_SEQUENCE, CMD_ANY, it);
                        break;

                    case T_ANYPATH:
                        next_token(it);
                        next = (out != NULL) ? out->sChildren.last() : NULL;
                        // Premature optimization: Reduce sequence of **/ to one element
                        if ((next != NULL) && (next->nCommand == CMD_ANYPATH))
                            break;
                        res = merge_simple(&out, CMD_SEQUENCE, CMD_ANYPATH, it);
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
                                out->nLength    = 0;
                                out->nChars     = 0;
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
                (*dst)->bInverse   ^= inverse;

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
            it.nLength      = 0;
            it.nChars       = 0;

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

        bool PathPattern::check_pattern_case(const lsp_wchar_t *pat, const lsp_wchar_t *s, size_t len)
        {
            lsp_wchar_t c, pc;

            for (size_t off=0; off<len; )
            {
                c = *(s++);             // Matching character
                pc = pat[off++];        // Pattern character

                switch (pc)
                {
                    case '/':
                    case '\\':
                        if ((c != '/') && (c != '\\'))
                            return false;
                        break;
                    case '?':
                        if ((c == '/') || (c == '\\'))
                            return false;
                        break;
                    case '`':
                        pc = (off < len) ? pat[off] : '`';
                        switch (pc)
                        {
                            // Special symbols
                            case '*': case '(': case ')': case '|':
                            case '&': case '!': case '`':
                                ++off;
                                break;
                            default:
                                pc = '`';
                                break;
                        }
                        if (c != pc)
                            return false;
                        break;

                    default:
                        if (c != pc)
                            return false;
                        break;
                } // switch
            }

            return true;
        }

        bool PathPattern::check_pattern_nocase(const lsp_wchar_t *pat, const lsp_wchar_t *s, size_t len)
        {
            lsp_wchar_t c, pc;

            for (size_t off=0; off<len; )
            {
                c = *(s++);             // Matching character
                pc = pat[off++];        // Pattern character

                switch (pc)
                {
                    case '/':
                    case '\\':
                        if ((c != '/') && (c != '\\'))
                            return false;
                        break;
                    case '?':
                        if ((c == '/') || (c == '\\'))
                            return false;
                        break;
                    case '`':
                        pc = (off < len) ? pat[off] : '`';
                        switch (pc)
                        {
                            // Special symbols
                            case '*': case '(': case ')': case '|':
                            case '&': case '!': case '`':
                                ++off;
                                break;
                            default:
                                pc = '`';
                                break;
                        }
                        if (towlower(c) != towlower(pc))
                            return false;
                        break;

                    default:
                        if (towlower(c) != towlower(pc))
                            return false;
                        break;
                } // switch
            }

            return true;
        }

        bool PathPattern::pattern_matcher_match(matcher_t *m, size_t start, size_t count)
        {
            const cmd_t *cmd        = m->cmd;
            if (count != m->cmd->nChars)
                return cmd->bInverse;

            const lsp_wchar_t *pat = m->pat->characters() + cmd->nStart;
            const lsp_wchar_t *str = m->str->characters() + start;
            bool match = (m->flags & MATCH_CASE) ?
                        check_pattern_case(pat, str, cmd->nLength) :
                        check_pattern_nocase(pat, str, cmd->nLength);

            return match ^ cmd->bInverse;
        }

        bool PathPattern::any_matcher_match(matcher_t *m, size_t start, size_t count)
        {
            const cmd_t *cmd        = m->cmd;
            if (count == 0)
                return !cmd->bInverse;

            any_matcher_t *am       = static_cast<any_matcher_t *>(m);
            if ((am->bad >= ssize_t(start)) && (am->bad < ssize_t(start + count)))
                return cmd->bInverse;

            // We need to ensure that there are no denied items within the area
            const lsp_wchar_t *str = m->str->characters() + start;
            for (size_t i=0; i<count; ++i)
            {
                lsp_wchar_t ch  = str[i];
                if ((ch == '/') || (ch == '\\'))
                {
                    am->bad         = start + i; // Cache last bad value
                    return cmd->bInverse;
                }
            }

            return !cmd->bInverse;
        }

        bool PathPattern::anypath_matcher_match(matcher_t *m, size_t start, size_t count)
        {
            const cmd_t *cmd        = m->cmd;
            const lsp_wchar_t *str  = m->str->characters() + start;
            lsp_wchar_t ch;

            // Not beginning of string?
            if (start > 0)
            {
                // Previous character in sequence should be a path separator
                ch                      = str[-1];
                if ((ch != '/') && (ch != '\\'))
                    return cmd->bInverse;

                // Zero length, previous character is separator -> matches
                if (count == 0)
                    return !cmd->bInverse;
            }
            else if (count == 0)
                return !cmd->bInverse; // Zero length, start of string -> matches

            // Last character in sequence should be a path separator
            ch  = str[count-1];
            if ((ch == '/') || (ch == '/'))
                return !cmd->bInverse;

            // The separator is not necessary if we are at the end of line
            if ((start + count) >= m->str->length())
                return !cmd->bInverse;

            return cmd->bInverse;
        }

        bool PathPattern::and_matcher_match(matcher_t *m, size_t start, size_t count)
        {
            bool_matcher_t *bm = static_cast<bool_matcher_t *>(m);
            const cmd_t *cmd        = m->cmd;

            // If at least one matcher does not match - return result
            for (size_t i=0, n=bm->cond.size(); i<n; ++i)
            {
                matcher_t *m = bm->cond.uget(i);
                if (!m->match(m, start, count))
                    return cmd->bInverse;
            }
            return !cmd->bInverse;
        }

        bool PathPattern::or_matcher_match(matcher_t *m, size_t start, size_t count)
        {
            bool_matcher_t *bm = static_cast<bool_matcher_t *>(m);
            const cmd_t *cmd        = m->cmd;

            // If at least one matcher matches - return result
            for (size_t i=0, n=bm->cond.size(); i<n; ++i)
            {
                matcher_t *m = bm->cond.uget(i);
                if (m->match(m, start, count))
                    return !cmd->bInverse;
            }
            return cmd->bInverse;
        }

        PathPattern::matcher_t *PathPattern::create_matcher(const matcher_t *src, const cmd_t *cmd)
        {
            switch (cmd->nCommand)
            {
                case CMD_PATTERN:
                {
                    matcher_t *m = new matcher_t();
                    if (m != NULL)
                    {
                        m->type             = M_PATTERN;
                        m->match            = pattern_matcher_match;
                        m->cmd              = cmd;
                        m->pat              = src->pat;
                        m->str              = src->str;
                        m->flags            = src->flags;
                    }
                    return m;
                }

                case CMD_ANY:
                {
                    any_matcher_t *m = new any_matcher_t();
                    if (m != NULL)
                    {
                        m->type             = M_ANY;
                        m->match            = any_matcher_match;
                        m->cmd              = cmd;
                        m->pat              = src->pat;
                        m->str              = src->str;
                        m->flags            = src->flags;
                        m->bad              = -1;
                    }
                    return m;
                }

                case CMD_ANYPATH:
                {
                    matcher_t *m = new matcher_t();
                    if (m != NULL)
                    {
                        m->type             = M_ANYPATH;
                        m->match            = anypath_matcher_match;
                        m->cmd              = cmd;
                        m->pat              = src->pat;
                        m->str              = src->str;
                        m->flags            = src->flags;
                    }
                    return m;
                }

                case CMD_AND:
                case CMD_OR:
                {
                    bool_matcher_t *m = new bool_matcher_t();
                    if (m != NULL)
                    {
                        m->type             = M_BOOL;
                        m->match            = (cmd->nCommand == CMD_AND) ? and_matcher_match : or_matcher_match;
                        m->cmd              = cmd;
                        m->pat              = src->pat;
                        m->str              = src->str;
                        m->flags            = src->flags;

                        // Create child matchers
                        for (size_t i=0, n=cmd->sChildren.size(); i<n; ++i)
                        {
                            const cmd_t *c      = cmd->sChildren.uget(i);
                            matcher_t *cm       = create_matcher(m, c);
                            if ((cm == NULL) || (!m->cond.add(cm)))
                            {
                                destroy_matcher(m);
                                return NULL;
                            }
                        }
                    }
                    return m;
                }

                default:
                    break;
            }

            return NULL;
        }

        void PathPattern::destroy_matcher(matcher_t *m)
        {
            if (m == NULL)
                return;

            switch (m->type)
            {
                // Pattern matcher
                case M_PATTERN:
                case M_ANY:
                case M_ANYPATH:
                {
                    delete m;
                    break;
                }

                case M_BOOL:
                {
                    bool_matcher_t *bm = static_cast<bool_matcher_t *>(m);

                    // Destroy all child matchers
                    for (size_t i=0, n=bm->cond.size(); i<n; ++i)
                        destroy_matcher(bm->cond.uget(i));

                    delete bm;
                    break;
                }

                default:
                    break;
            }
        }

        bool PathPattern::match_full(const LSPString *path) const
        {
            matcher_t root;
            root.cmd            = NULL;
            root.pat           = &sMask;
            root.str            = path;
            root.flags          = nFlags & MATCH_CASE;

            matcher_t *rm       = create_matcher(&root, pRoot);
            if (rm == NULL)
                return false;

            // Seek the matcher
            bool match      = rm->match(rm, 0, path->length()) ^ bool(nFlags & INVERSE);

            destroy_matcher(rm);

            return match;
        }

        status_t PathPattern::set(const char *pattern, size_t flags)
        {
            LSPString tmp;
            return (tmp.set_utf8(pattern)) ? parse(&tmp, flags) : STATUS_NO_MEM;
        }

        size_t PathPattern::set_flags(size_t flags)
        {
            size_t old  = nFlags;
            nFlags      = flags & (INVERSE | MATCH_CASE | FULL_PATH);
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
