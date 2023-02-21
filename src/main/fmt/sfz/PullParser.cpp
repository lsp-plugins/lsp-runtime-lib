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

#include <lsp-plug.in/fmt/sfz/PullParser.h>
#include <lsp-plug.in/io/InFileStream.h>
#include <lsp-plug.in/io/InMemoryStream.h>
#include <lsp-plug.in/io/InStringSequence.h>
#include <lsp-plug.in/io/InSequence.h>

namespace lsp
{
    namespace sfz
    {

        PullParser::PullParser()
        {
            pIn             = NULL;
            nWFlags         = 0;
            sCurrent.type   = EVENT_NONE;
            nUnget          = 0;
        }

        PullParser::~PullParser()
        {
            close();
        }

        status_t PullParser::open(const char *path, const char *charset)
        {
            if (pIn != NULL)
                return STATUS_OPENED;

            io::InFileStream *ifs = new io::InFileStream();
            if (ifs == NULL)
                return STATUS_NO_MEM;

            status_t res = ifs->open(path);
            if (res == STATUS_OK)
                res         = wrap(ifs, WRAP_CLOSE | WRAP_DELETE, charset);
            if (res != STATUS_OK)
            {
                ifs->close();
                delete ifs;
            }

            return res;
        }

        status_t PullParser::open(const LSPString *path, const char *charset)
        {
            if (pIn != NULL)
                return STATUS_OPENED;

            io::InFileStream *ifs = new io::InFileStream();
            if (ifs == NULL)
                return STATUS_NO_MEM;

            status_t res = ifs->open(path);
            if (res == STATUS_OK)
                res         = wrap(ifs, WRAP_CLOSE | WRAP_DELETE, charset);
            if (res != STATUS_OK)
            {
                ifs->close();
                delete ifs;
            }

            return res;
        }

        status_t PullParser::open(const io::Path *path, const char *charset)
        {
            if (pIn != NULL)
                return STATUS_OPENED;

            io::InFileStream *ifs = new io::InFileStream();
            if (ifs == NULL)
                return STATUS_NO_MEM;

            status_t res = ifs->open(path);
            if (res == STATUS_OK)
                res         = wrap(ifs, WRAP_CLOSE | WRAP_DELETE, charset);
            if (res != STATUS_OK)
            {
                ifs->close();
                delete ifs;
            }

            return res;
        }

        status_t PullParser::wrap(const char *str, const char *charset)
        {
            if (pIn != NULL)
                return STATUS_OPENED;

            io::InMemoryStream *ifs = new io::InMemoryStream(str, strlen(str));
            if (ifs == NULL)
                return STATUS_NO_MEM;

            status_t res = wrap(ifs, WRAP_CLOSE | WRAP_DELETE, charset);
            if (res != STATUS_OK)
            {
                ifs->close();
                delete ifs;
            }

            return res;
        }

        status_t PullParser::wrap(const LSPString *str)
        {
            if (pIn != NULL)
                return STATUS_OPENED;

            io::IInSequence *is = new io::InStringSequence(str);
            if (is == NULL)
                return STATUS_NO_MEM;

            status_t res = wrap(is, WRAP_CLOSE | WRAP_DELETE);
            if (res != STATUS_OK)
            {
                is->close();
                delete is;
            }
            return res;
        }

        status_t PullParser::wrap(io::IInSequence *seq, size_t flags)
        {
            if (pIn != NULL)
                return STATUS_OPENED;

            pIn             = seq;
            nWFlags         = flags;
            sCurrent.type   = EVENT_NONE;
            sUnget.truncate();
            nUnget          = 0;

            return STATUS_OK;
        }

        status_t PullParser::wrap(io::IInStream *is, size_t flags, const char *charset)
        {
            if (pIn != NULL)
                return STATUS_OPENED;

            io::InSequence *isq = new io::InSequence();
            if (isq == NULL)
                return STATUS_NO_MEM;

            status_t res = isq->wrap(is, flags, (charset == NULL) ? "UTF-8" : charset);
            if (res == STATUS_OK)
                res         = wrap(isq, WRAP_CLOSE | WRAP_DELETE);

            if (res != STATUS_OK)
            {
                isq->close();
                delete isq;
            }

            return res;
        }

        status_t PullParser::close()
        {
            status_t res    = STATUS_OK;
            if (pIn == NULL)
                return res;

            if (nWFlags & WRAP_CLOSE)
                res = pIn->close();
            if (nWFlags & WRAP_DELETE)
                delete pIn;

            pIn             = NULL;
            sCurrent.type   = EVENT_NONE;
            sUnget.truncate();
            nUnget          = 0;

            return res;
        }

        const event_t *PullParser::current() const
        {
            return &sCurrent;
        }

        status_t PullParser::next(event_t *event)
        {
            if (pIn == NULL)
                return STATUS_CLOSED;

            // Read the next event
            status_t res = read_next_event(&sCurrent);
            if (res != STATUS_OK)
                return res;

            // Return event data if operation was successful
            if (event != NULL)
            {
                LSPString name, value;
                if (!name.set(&sCurrent.name))
                    return STATUS_NO_MEM;
                if (!value.set(&sCurrent.value))
                    return STATUS_NO_MEM;

                // Commit changes
                event->type         = sCurrent.type;
                event->name.swap(&name);
                event->value.swap(&value);
            }

            return STATUS_OK;
        }

        bool PullParser::is_identifier(lsp_wchar_t ch, bool first)
        {
            if ((ch >= 'a') && (ch <= 'z'))
                return true;
            if ((ch >= 'A') && (ch <= 'Z'))
                return true;
            if (ch == '_')
                return true;
            if ((!first) && (ch >= '0') && (ch <= '9'))
                return true;
            return false;
        }

        bool PullParser::is_space(lsp_wchar_t ch)
        {
            switch (ch)
            {
                case '\n':
                case '\r':
                case '\t':
                case '\v':
                case ' ':
                    return true;
                default:
                    break;
            }
            return false;
        }

        void PullParser::trim_right(LSPString *value)
        {
            ssize_t index = ssize_t(value->length()) - 1;
            for (; index >= 0; --index)
            {
                if (!is_space(value->char_at(index)))
                    break;
            }
            value->set_length((index >= 0) ? index + 1 : 0);
        }

        lsp_swchar_t PullParser::get_char()
        {
            if (nUnget < sUnget.length())
            {
                lsp_wchar_t ch = sUnget.char_at(nUnget++);
                if (nUnget >= sUnget.length())
                    sUnget.truncate();
                return ch;
            }
            return pIn->read();
        }

        status_t PullParser::read_next_event(event_t *ev)
        {
            lsp_swchar_t ch;

            while ((ch = get_char()) >= 0)
            {
                switch (ch)
                {
                    case '<':
                        return set_error(read_header(ev));
                    case '/':
                        return set_error(read_comment(ev));
                    case '#':
                        return set_error(read_preprocessor(ev));
                    default:
                        if (is_space(ch))
                            break;
                        return set_error(read_opcode(ch, ev));
                }
            }

            return set_error(-ch);
        }

        status_t PullParser::read_header(event_t *ev)
        {
            lsp_swchar_t ch;
            LSPString header;

            while ((ch = get_char()) >= 0)
            {
                if (ch == '>')
                {
                    ev->type        = EVENT_HEADER;
                    ev->name.swap(&header);
                    ev->value.clear();

                    return STATUS_OK;
                }
                else if (!is_identifier(ch, header.is_empty()))
                    return STATUS_CORRUPTED;

                if (!header.append(ch))
                    return STATUS_NO_MEM;
            }

            return (ch == -STATUS_EOF) ? STATUS_CORRUPTED : -ch;
        }

        status_t PullParser::read_opcode_name(lsp_swchar_t ch, LSPString *name)
        {
            if (!is_identifier(ch, true))
                return STATUS_CORRUPTED;
            if (!name->append(ch))
                return STATUS_NO_MEM;

            while ((ch = get_char()) >= 0)
            {
                if (ch == '=')
                    return STATUS_OK;
                else if (!is_identifier(ch, name->is_empty()))
                    return STATUS_CORRUPTED;

                if (!name->append(ch))
                    return STATUS_NO_MEM;
            }

            return (ch == -STATUS_EOF) ? STATUS_CORRUPTED : -ch;
        }

        status_t PullParser::read_opcode_value(LSPString *value)
        {
            lsp_swchar_t ch;

            while ((ch = get_char()) >= 0)
            {
                switch (ch)
                {
                    case '<': // header?
                    case '/': // comment?
                        nUnget = 0;
                        sUnget.clear();
                        if (!sUnget.append(ch))
                            return STATUS_NO_MEM;
                        return STATUS_OK;

                    default:
                        if (is_space(ch))
                            return STATUS_OK;
                        if (!value->append(ch))
                            return STATUS_NO_MEM;
                        break;
                }
            }

            return (ch == -STATUS_EOF) ? STATUS_OK : -ch;
        }

        status_t PullParser::read_sample_file_name(LSPString *value)
        {
            lsp_swchar_t ch;

            while ((ch = get_char()) >= 0)
            {
                if (ch == '=')
                {
                    // Special case from SFZ specification:
                    // Long names and names with blank spaces and other special characters
                    // (excepting the = character) are allowed in the sample definition.
                    // sample=some file name.ext opcode2= <- we are here
                    if (!value->append(ch))
                        return STATUS_NO_MEM;

                    // Look for the first space character from the end
                    ssize_t index = ssize_t(value->length()) - 2;
                    for (; index >= 0; --index)
                    {
                        ch = value->char_at(index);
                        if (is_space(ch))
                            break;
                    }

                    // Fill the unget buffer
                    if (!sUnget.set(value, (index >= 0) ? index + 1 : 0))
                        return STATUS_NO_MEM;
                    nUnget      = 0;

                    // Trim the value from the right
                    value->set_length((index >= 0) ? index : 0);
                    trim_right(value);

                    return STATUS_OK;
                }
                else if (ch == '\n')
                {
                    trim_right(value);
                    return STATUS_OK;
                }

                if (!value->append(ch))
                    return STATUS_NO_MEM;
            }

            return (ch == -STATUS_EOF) ? STATUS_OK : -ch;
        }

        status_t PullParser::read_opcode(lsp_wchar_t ch, event_t *ev)
        {
            status_t res;
            LSPString name, value;

            if ((res = read_opcode_name(ch, &name)) != STATUS_OK)
                return res;

            if (name.equals_ascii("sample"))
            {
                if ((res = read_sample_file_name(&value)) != STATUS_OK)
                    return res;
            }
            else
            {
                if ((res = read_opcode_value(&value)) != STATUS_OK)
                    return res;
            }

            // Commit the result
            ev->type        = EVENT_OPCODE;
            ev->name.swap(&name);
            ev->value.swap(&value);
            ev->blob.close();

            return STATUS_OK;
        }

        status_t PullParser::read_comment(event_t *ev)
        {
            lsp_swchar_t ch;
            if ((ch = get_char()) < 0)
                return -ch;
            if (ch != '/')
                return STATUS_CORRUPTED;

            // Read the entire string until end of line
            LSPString text;
            while ((ch = get_char()) >= 0)
            {
                if (ch == '\n')
                {
                    // Patch for Windows line endings
                    if (text.last() == '\r')
                        text.remove_last();
                    break;
                }

                if (!text.append(ch))
                    return STATUS_NO_MEM;
            }

            ev->type        = EVENT_COMMENT;
            ev->name.clear();
            ev->value.swap(&text);
            ev->blob.close();

            return STATUS_OK;
        }

        status_t PullParser::expect_string(const char *text)
        {
            // Read the expected string from the file
            lsp_swchar_t ch;
            for (const char *p=text; *p != '\0'; ++p)
            {
                if ((ch = get_char()) < 0)
                    return (ch == -STATUS_EOF) ? STATUS_CORRUPTED : -ch;
                if (ch != *p)
                    return STATUS_CORRUPTED;
            }

            return STATUS_OK;
        }

        status_t PullParser::expect_char(lsp_swchar_t expected)
        {
            lsp_swchar_t ch;
            while ((ch = get_char()) >= 0)
            {
                if (ch == expected)
                    return STATUS_OK;
                if (!is_space(ch))
                    return STATUS_CORRUPTED;
            }

            return (ch == -STATUS_EOF) ? STATUS_CORRUPTED : -ch;
        }

        status_t PullParser::read_variable_name(LSPString *value)
        {
            lsp_swchar_t ch;
            if (!value->append('$'))
                return STATUS_NO_MEM;

            while ((ch = get_char()) >= 0)
            {
                if (is_space(ch))
                    break;
                if (!is_identifier(ch, value->length() == 1))
                    return STATUS_CORRUPTED;
                if (!value->append(ch))
                    return STATUS_NO_MEM;
            }
            if ((ch < 0) && (ch != -STATUS_EOF))
                return -ch;

            // The variable name should be of at least 1 character after '$' sign
            return (value->length() >= 2) ? STATUS_OK : STATUS_CORRUPTED;
        }

        status_t PullParser::read_variable_value(LSPString *value)
        {
            lsp_swchar_t ch;

            // Read first character
            while ((ch = get_char()) >= 0)
            {
                if (!is_space(ch))
                    break;
            }
            if (ch < 0)
                return (ch != -STATUS_EOF) ? -ch : STATUS_OK;

            if (!value->append(ch))
                return STATUS_NO_MEM;

            // Read the last variable definition
            while ((ch = get_char()) >= 0)
            {
                switch (ch)
                {
                    case '<': // header?
                    case '/': // comment?
                        nUnget = 0;
                        sUnget.clear();
                        if (!sUnget.append(ch))
                            return STATUS_NO_MEM;
                        return STATUS_OK;

                    default:
                        if (is_space(ch))
                            return STATUS_OK;
                        if (!value->append(ch))
                            return STATUS_NO_MEM;
                        break;
                }
            }

            return (ch == -STATUS_EOF) ? STATUS_OK : -ch;
        }

        status_t PullParser::read_define(event_t *ev)
        {
            status_t res;
            if ((res = expect_string("efine")) != STATUS_OK) // 'd' already was read
                return res;
            if ((res = expect_char('$')) != STATUS_OK)
                return res;

            // Read the variable name
            LSPString name;
            if ((res = read_variable_name(&name) != STATUS_OK))
                return res;

            // Read the variable value
            LSPString value;
            if ((res = read_variable_value(&value) != STATUS_OK))
                return res;

            // Commit the result
            ev->type        = EVENT_DEFINE;
            ev->name.swap(&name);
            ev->value.swap(&value);
            ev->blob.close();

            return STATUS_OK;
        }

        status_t PullParser::read_include(event_t *ev)
        {
            status_t res;
            if ((res = expect_string("nclude")) != STATUS_OK) // 'i' already was read
                return res;
            if ((res = expect_char('\"')) != STATUS_OK)
                return res;

            // Read the entire string until the end quote occurs
            lsp_swchar_t ch;
            LSPString text;
            while (true)
            {
                if ((ch = get_char()) < 0)
                    return (ch == -STATUS_EOF) ? -STATUS_CORRUPTED : -ch;

                if (ch == '\"')
                    break;
                if (!text.append(ch))
                    return STATUS_NO_MEM;
            }

            // Commit the result
            ev->type        = EVENT_INCLUDE;
            ev->name.clear();
            ev->value.swap(&text);
            ev->blob.close();

            return STATUS_OK;
        }

        status_t PullParser::read_preprocessor(event_t *ev)
        {
            lsp_swchar_t ch = get_char();
            switch (ch)
            {
                case 'd': return read_define(ev);
                case 'i': return read_include(ev);
                default: break;
            }

            return ((ch < 0) && (ch != -STATUS_EOF)) ? -ch : STATUS_CORRUPTED;
        }

        status_t PullParser::set_error(status_t code)
        {
            if (code != STATUS_OK)
            {
                sCurrent.type       = EVENT_NONE;
                sCurrent.name.truncate();
                sCurrent.value.truncate();
            }

            return code;
        }

    } /* namespace sfz */
} /* namespace lsp */



