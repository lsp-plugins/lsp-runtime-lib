/*
 * Copyright (C) 2024 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2024 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-runtime-lib
 * Created on: 17 окт. 2019 г.
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

#include <lsp-plug.in/io/File.h>
#include <lsp-plug.in/io/OutFileStream.h>
#include <lsp-plug.in/io/OutStringSequence.h>
#include <lsp-plug.in/io/OutSequence.h>
#include <lsp-plug.in/fmt/json/Tokenizer.h>
#include <lsp-plug.in/fmt/json/Serializer.h>
#include <lsp-plug.in/stdlib/stdio.h>
#include <lsp-plug.in/stdlib/math.h>
#include <lsp-plug.in/stdlib/locale.h>

namespace lsp
{
    namespace json
    {
        
        Serializer::Serializer()
        {
            pOut            = NULL;
            nWFlags         = 0;
            sState.mode     = WRITE_ROOT;
            sState.flags    = 0;
            sState.ident    = 0;

            init_serial_flags(&sSettings);
        }
        
        Serializer::~Serializer()
        {
            close();
        }

        status_t Serializer::open(const char *path, const serial_flags_t *settings, const char *charset)
        {
            if (pOut != NULL)
                return STATUS_BAD_STATE;
            else if (path == NULL)
                return STATUS_BAD_ARGUMENTS;

            io::OutFileStream *ofs = new io::OutFileStream();
            if (ofs == NULL)
                return STATUS_NO_MEM;
            status_t res = ofs->open(path, io::File::FM_WRITE | io::File::FM_TRUNC | io::File::FM_CREATE);
            if (res == STATUS_OK)
            {
                res     = wrap(ofs, settings, WRAP_CLOSE | WRAP_DELETE, charset);
                if (res == STATUS_OK)
                    return res;
                ofs->close();
            }
            delete ofs;

            return res;
        }

        status_t Serializer::open(const LSPString *path, const serial_flags_t *settings, const char *charset)
        {
            if (pOut != NULL)
                return STATUS_BAD_STATE;
            else if (path == NULL)
                return STATUS_BAD_ARGUMENTS;

            io::OutFileStream *ofs = new io::OutFileStream();
            if (ofs == NULL)
                return STATUS_NO_MEM;
            status_t res = ofs->open(path, io::File::FM_WRITE | io::File::FM_TRUNC | io::File::FM_CREATE);
            if (res == STATUS_OK)
            {
                res     = wrap(ofs, settings, WRAP_CLOSE | WRAP_DELETE, charset);
                if (res == STATUS_OK)
                    return res;
                ofs->close();
            }
            delete ofs;

            return res;
        }

        status_t Serializer::open(const io::Path *path, const serial_flags_t *settings, const char *charset)
        {
            if (pOut != NULL)
                return STATUS_BAD_STATE;
            else if (path == NULL)
                return STATUS_BAD_ARGUMENTS;

            io::OutFileStream *ofs = new io::OutFileStream();
            if (ofs == NULL)
                return STATUS_NO_MEM;
            status_t res = ofs->open(path, io::File::FM_WRITE | io::File::FM_TRUNC | io::File::FM_CREATE);
            if (res == STATUS_OK)
            {
                res     = wrap(ofs, settings, WRAP_CLOSE | WRAP_DELETE, charset);
                if (res == STATUS_OK)
                    return res;
                ofs->close();
            }
            delete ofs;

            return res;
        }

        void Serializer::copy_settings(const serial_flags_t *flags)
        {
            if (flags == NULL)
                init_serial_flags(&sSettings);
            else
                sSettings       = *flags;
        }

        status_t Serializer::wrap(LSPString *str, const serial_flags_t *settings)
        {
            if (pOut != NULL)
                return STATUS_BAD_STATE;
            else if (str == NULL)
                return STATUS_BAD_ARGUMENTS;

            io::OutStringSequence *seq = new io::OutStringSequence(str, false);
            if (seq == NULL)
                return STATUS_NO_MEM;

            status_t res = wrap(seq, settings, WRAP_CLOSE | WRAP_DELETE);
            if (res == STATUS_OK)
                return res;

            seq->close();
            delete seq;

            return res;
        }

        status_t Serializer::wrap(io::IOutStream *os, const serial_flags_t *settings, size_t flags, const char *charset)
        {
            if (pOut != NULL)
                return STATUS_BAD_STATE;
            else if (os == NULL)
                return STATUS_BAD_ARGUMENTS;

            io::OutSequence *seq = new io::OutSequence();
            if (seq == NULL)
                return STATUS_NO_MEM;

            status_t res = seq->wrap(os, flags, charset);
            if (res == STATUS_OK)
            {
                if ((res = wrap(seq, settings, WRAP_CLOSE | WRAP_DELETE)) == STATUS_OK)
                    return res;
                seq->close();
            }

            delete seq;
            return res;
        }

        status_t Serializer::wrap(io::IOutSequence *seq, const serial_flags_t *settings, size_t flags)
        {
            if (pOut != NULL)
                return STATUS_BAD_STATE;
            else if (seq == NULL)
                return STATUS_BAD_ARGUMENTS;

            pOut            = seq;
            nWFlags         = flags;
            sState.mode     = WRITE_ROOT;
            sState.flags    = 0;
            copy_settings(settings);

            return STATUS_OK;
        }

        status_t Serializer::close()
        {
            status_t res = STATUS_OK;

            // Close handles
            if (pOut != NULL)
            {
                if (nWFlags & WRAP_CLOSE)
                    res     = pOut->close();

                if (nWFlags & WRAP_DELETE)
                    delete pOut;

                pOut = NULL;
            }

            sStack.flush();

            return res;
        }

        status_t Serializer::push_state(pmode_t mode)
        {
            if (!sStack.add(&sState))
                return STATUS_NO_MEM;

            sState.mode     = mode;
            sState.flags    = 0;
            sState.ident   += sSettings.padding;
            return STATUS_OK;
        }

        status_t Serializer::pop_state()
        {
            state_t *st = sStack.last();
            if (st == NULL)
                return STATUS_BAD_STATE;

            sState          = *st;
            return (sStack.pop()) ? STATUS_OK : STATUS_BAD_STATE;
        }

        status_t Serializer::write_comma()
        {
            if (pOut == NULL)
                return STATUS_BAD_STATE;

            switch (sState.mode)
            {
                case WRITE_ROOT:
                    return STATUS_INVALID_VALUE;
                case WRITE_ARRAY:
                case WRITE_OBJECT:
                    if ((sState.flags & (SF_VALUE | SF_COMMA)) != SF_VALUE)
                        return STATUS_INVALID_VALUE;
                    break;
                default:
                    return STATUS_BAD_STATE;
            }

            sState.flags   |= SF_COMMA;
            return pOut->write(',');
        }

        status_t Serializer::writeln()
        {
            if (!sSettings.multiline)
                return STATUS_OK;

            sState.flags   &= ~SF_CONTENT;
            status_t res = pOut->write('\n');
            if (res != STATUS_OK)
                return res;
            for (size_t i=0, n=sState.ident; i<n; ++i)
            {
                if ((res = pOut->write(sSettings.ident)) != STATUS_OK)
                    break;
            }

            return res;
        }

        char Serializer::hex(int x)
        {
            return ((x &= 0x0f) < 10) ? (x + '0') : (x + 'A' - 10);
        }

        status_t Serializer::write(const event_t *event)
        {
            if (event == NULL)
                return STATUS_BAD_ARGUMENTS;

            switch (event->type)
            {
                case JE_OBJECT_START:   return start_object();
                case JE_OBJECT_END:     return end_object();
                case JE_ARRAY_START:    return start_array();
                case JE_ARRAY_END:      return end_array();
                case JE_PROPERTY:       return write_property(&event->sValue);
                case JE_STRING:         return write_string(&event->sValue);
                case JE_INTEGER:        return write_int(lsp::fixed_int(event->iValue));
                case JE_DOUBLE:         return write_double(event->fValue);
                case JE_BOOL:           return write_bool(event->bValue);
                case JE_NULL:           return write_null();
                default:                break;
            }

            return STATUS_BAD_ARGUMENTS;
        }

        status_t Serializer::emit_comma()
        {
            if ((sState.flags & (SF_VALUE | SF_COMMA)) != SF_VALUE)
                return STATUS_OK;
            sState.flags |= SF_CONTENT;
            return pOut->write(',');
        }

        status_t Serializer::emit_separator()
        {
            if ((!sSettings.separator) || (!(sState.flags & SF_CONTENT)))
                return STATUS_OK;
            return pOut->write(' ');
        }

        status_t Serializer::write_raw(const char *buf, int len)
        {
            status_t res = STATUS_OK;

            switch (sState.mode)
            {
                case WRITE_ROOT:
                    if (sState.flags & SF_VALUE)
                        return STATUS_INVALID_VALUE;
                    break;
                case WRITE_ARRAY:
                    if ((res = emit_comma()) == STATUS_OK)
                        res = writeln();
                    break;
                case WRITE_OBJECT:
                    if (!(sState.flags & SF_PROPERTY))
                        return STATUS_INVALID_VALUE;
                    sState.flags &= ~SF_PROPERTY;
                    break;
                default:
                    return STATUS_BAD_STATE;
            }
            if (res == STATUS_OK)
                res = emit_separator();

            sState.flags   &= ~SF_COMMA;
            sState.flags   |= SF_VALUE | SF_CONTENT;

            return (res == STATUS_OK) ? pOut->write_ascii(buf, len) : res;
        }

        status_t Serializer::write_int(int32_t value)
        {
            if (pOut == NULL)
                return STATUS_BAD_STATE;

            char buf[0x20];
            int len = ::snprintf(buf, sizeof(buf), "%ld", (long)value);
            return (len < int(sizeof(buf))) ? write_raw(buf, len) : STATUS_OVERFLOW;
        }

        status_t Serializer::write_int(uint32_t value)
        {
            if (pOut == NULL)
                return STATUS_BAD_STATE;

            char buf[0x20];
            int len = ::snprintf(buf, sizeof(buf), "%lu", (unsigned long)value);
            return (len < int(sizeof(buf))) ? write_raw(buf, len) : STATUS_OVERFLOW;
        }

        status_t Serializer::write_int(int64_t value)
        {
            if (pOut == NULL)
                return STATUS_BAD_STATE;

            char buf[0x20];
            int len = ::snprintf(buf, sizeof(buf), "%lld", (long long)value);
            return (len < int(sizeof(buf))) ? write_raw(buf, len) : STATUS_OVERFLOW;
        }

        status_t Serializer::write_int(uint64_t value)
        {
            if (pOut == NULL)
                return STATUS_BAD_STATE;

            char buf[0x20];
            int len = ::snprintf(buf, sizeof(buf), "%llu", (unsigned long long)value);
            return (len < int(sizeof(buf))) ? write_raw(buf, len) : STATUS_OVERFLOW;
        }

        status_t Serializer::write_hex(int32_t value)
        {
            if (pOut == NULL)
                return STATUS_BAD_STATE;
            else if (sSettings.version < JSON_VERSION5)
                return STATUS_INVALID_VALUE;

            char buf[0x20];
            int len = (value < 0) ?
                    ::snprintf(buf, sizeof(buf), "-0x%lx", (long)(-value)) :
                    ::snprintf(buf, sizeof(buf), "0x%lx", (long)value);
            return (len < int(sizeof(buf))) ? write_raw(buf, len) : STATUS_OVERFLOW;
        }

        status_t Serializer::write_hex(uint32_t value)
        {
            if (pOut == NULL)
                return STATUS_BAD_STATE;
            else if (sSettings.version < JSON_VERSION5)
                return STATUS_INVALID_VALUE;

            char buf[0x20];
            int len = ::snprintf(buf, sizeof(buf), "0x%lx", (unsigned long)value);
            return (len < int(sizeof(buf))) ? write_raw(buf, len) : STATUS_OVERFLOW;
        }

        status_t Serializer::write_hex(int64_t value)
        {
            if (pOut == NULL)
                return STATUS_BAD_STATE;
            else if (sSettings.version < JSON_VERSION5)
                return STATUS_INVALID_VALUE;

            char buf[0x20];
            int len = (value < 0) ?
                    ::snprintf(buf, sizeof(buf), "-0x%llx", (long long)(-value)) :
                    ::snprintf(buf, sizeof(buf), "0x%llx", (long long)value);
            return (len < int(sizeof(buf))) ? write_raw(buf, len) : STATUS_OVERFLOW;
        }

        status_t Serializer::write_hex(uint64_t value)
        {
            if (pOut == NULL)
                return STATUS_BAD_STATE;
            else if (sSettings.version < JSON_VERSION5)
                return STATUS_INVALID_VALUE;

            char buf[0x20];
            int len = ::snprintf(buf, sizeof(buf), "0x%llx", (unsigned long long)value);
            return (len < int(sizeof(buf))) ? write_raw(buf, len) : STATUS_OVERFLOW;
        }

        status_t Serializer::write_double(double value)
        {
            if (pOut == NULL)
                return STATUS_BAD_STATE;

            if (isnan(value))
                return write_raw("NaN", 3);
            else if (isinf(value))
                return (value < 0.0) ? write_raw("-Infinity", 9) : write_raw("Infinity", 8);

            char buf[0x20];
            const char *fmt = (sSettings.fmt_double != NULL) ? sSettings.fmt_double : "%f";
            int len = ::snprintf(buf, sizeof(buf), fmt, value);
            return (len < int(sizeof(buf))) ? write_raw(buf, len) : STATUS_OVERFLOW;
        }

        status_t Serializer::write_double(double value, const char *fmt)
        {
            if (pOut == NULL)
                return STATUS_BAD_STATE;

            if (isnan(value))
                return write_raw("NaN", 3);
            else if (isinf(value))
                return (value < 0.0) ? write_raw("-Infinity", 9) : write_raw("Infinity", 8);

            SET_LOCALE_SCOPED(LC_NUMERIC, "C"); // For proper decimal point

            char *buf = NULL;
            int len = asprintf(&buf, fmt, value);
            if (buf == NULL)
                return STATUS_NO_MEM;

            status_t res = (len >= 0) ? write_raw(buf, len) : STATUS_NO_DATA;
            ::free(buf);
            return res;
        }

        status_t Serializer::write_bool(bool value)
        {
            if (pOut == NULL)
                return STATUS_BAD_STATE;
            return (value) ? write_raw("true", 4) : write_raw("false", 5);
        }

        status_t Serializer::write_null()
        {
            if (pOut == NULL)
                return STATUS_BAD_STATE;
            return write_raw("null", 4);
        }

        status_t Serializer::write_string(const char *value, const char *charset)
        {
            if (value == NULL)
                return write_null();

            LSPString tmp;
            return (tmp.set_native(value, charset)) ? write_string(&tmp) : STATUS_NO_MEM;
        }

        status_t Serializer::write_string(const char *value)
        {
            if (value == NULL)
                return write_null();

            LSPString tmp;
            return (tmp.set_utf8(value)) ? write_string(&tmp) : STATUS_NO_MEM;
        }

        status_t Serializer::write_string(const LSPString *value)
        {
            if (value == NULL)
                return write_null();
            else if (pOut == NULL)
                return STATUS_BAD_STATE;

            // Analyze state
            status_t res = STATUS_OK;

            switch (sState.mode)
            {
                case WRITE_ROOT:
                    if (sState.flags & SF_VALUE)
                        return STATUS_INVALID_VALUE;
                    break;
                case WRITE_ARRAY:
                    if ((res = emit_comma()) == STATUS_OK)
                        res = writeln();
                    break;
                case WRITE_OBJECT:
                    if (!(sState.flags & SF_PROPERTY))
                        return STATUS_INVALID_VALUE;
                    sState.flags &= ~SF_PROPERTY;
                    break;
                default:
                    return STATUS_BAD_STATE;
            }
            if (res == STATUS_OK)
                res = emit_separator();

            sState.flags   &= ~SF_COMMA;
            sState.flags   |= SF_VALUE | SF_CONTENT;

            return (res == STATUS_OK) ? write_literal(value) : res;
        }

        status_t Serializer::write_literal(const LSPString *value)
        {
            status_t res;
            size_t last = 0, curr = 0, bl = 1;
            char xb[0x10];
            xb[0] = '\\';

            // Output start quote
            sState.flags |= SF_CONTENT;
            if ((res = pOut->write('\"')) != STATUS_OK)
                return res;

            for (size_t n=value->length(); curr<n; ++curr)
            {
                lsp_wchar_t ch = value->char_at(curr);

                // Check that we need to escape character
                switch (ch)
                {
                    case '\0': xb[bl++] = '0'; break;
                    case '\b': xb[bl++] = 'b'; break;
                    case '\f': xb[bl++] = 'f'; break;
                    case '\r': xb[bl++] = 'r'; break;
                    case '\t': xb[bl++] = 't'; break;
                    case '\v': xb[bl++] = 'v'; break;
                    case '\n': xb[bl++] = 'n'; break;
                    case '\\': xb[bl++] = '\\'; break;
                    case '\"': xb[bl++] = '\"'; break;
                    default:
                        if (ch < 0x20)
                        {
                            xb[1]   = 'u';
                            xb[2]   = '0';
                            xb[3]   = '0';
                            xb[4]   = hex(ch >> 4);
                            xb[5]   = hex(ch);
                            bl      = 6;
                        }
                        else if (ch >= 0x10000)
                        {
                            ch     -= 0x10000;
                            lsp_wchar_t hi = 0xd800 | (ch >> 10);
                            lsp_wchar_t lo = 0xdc00 | (ch & 0x3ff);

                            xb[1]   = 'u';
                            xb[2]   = hex(hi >> 12);
                            xb[3]   = hex(hi >> 8);
                            xb[4]   = hex(hi >> 4);
                            xb[5]   = hex(hi);
                            xb[6]   = '\\';
                            xb[7]   = 'u';
                            xb[8]   = hex(lo >> 12);
                            xb[9]   = hex(lo >> 8);
                            xb[10]  = hex(lo >> 4);
                            xb[11]  = hex(lo);
                            bl      = 12;
                        }
                        break;
                }

                // Is there extra escape sequence?
                if (bl > 1)
                {
                    // Write string directly to output
                    if (last < curr)
                    {
                        if ((res = pOut->write(value, last, curr)) != STATUS_OK)
                            return res;
                    }

                    // Append escape sequence and reset extra buffer size
                    if ((res = pOut->write_ascii(xb, bl)) != STATUS_OK)
                        return res;

                    last = curr+1; // Put buffer pointer to the next character
                    bl = 1;
                }
            }

            // Need to emit extra data?
            if (last < curr)
            {
                if ((res = pOut->write(value, last, curr)) != STATUS_OK)
                    return res;
            }

            // Output end quote
            return pOut->write('\"');
        }

        status_t Serializer::write_comment(const char *value, const char *charset)
        {
            if (value == NULL)
                return STATUS_BAD_ARGUMENTS;
            LSPString tmp;
            return (tmp.set_native(value, charset)) ? write_comment(&tmp) : STATUS_NO_MEM;
        }

        status_t Serializer::write_comment(const char *value)
        {
            if (value == NULL)
                return STATUS_BAD_ARGUMENTS;
            LSPString tmp;
            return (tmp.set_utf8(value)) ? write_comment(&tmp) : STATUS_NO_MEM;
        }

        status_t Serializer::write_comment(const LSPString *value)
        {
            if (pOut == NULL)
                return STATUS_BAD_STATE;
            else if (value == NULL)
                return STATUS_BAD_ARGUMENTS;
            else if (sSettings.version < JSON_VERSION5)
                return STATUS_INVALID_VALUE;

            // Comment can be placed everywhere, we don't need to analyze state
            status_t res;
            lsp_swchar_t prev = -1;
            size_t last = 0, curr = 0, bl = 4;
            char xb[0x10];
            xb[0] = '\\';
            xb[1] = 'u';
            xb[2] = '0';
            xb[3] = '0';

            // Output start quote
            if ((res = emit_separator()) != STATUS_OK)
                return res;

            sState.flags |= SF_CONTENT;
            if ((res = pOut->write_ascii("/*", 2)) != STATUS_OK)
                return res;

            for (size_t n=value->length(); curr<n; ++curr)
            {
                lsp_wchar_t ch = value->char_at(curr);

                switch (ch)
                {
                    case '\n':
                    case '\r':
                        if (sSettings.multiline)
                            break;
                        xb[bl++]    = hex(ch >> 4);
                        xb[bl++]    = hex(ch);
                        break;
                    case '/':
                        if (prev != '*')
                            break;
                        // Asterisk code in unicode
                        xb[bl++]    = '2';
                        xb[bl++]    = 'F';
                        break;
                    case '*':
                        if (prev != '/')
                            break;
                        // Asterisk code in unicode
                        xb[bl++]    = '2';
                        xb[bl++]    = 'A';
                        break;
                    default:
                        if (ch < 0x20)
                        {
                            xb[bl++] = hex(ch >> 4);
                            xb[bl++] = hex(ch);
                        }
                        break;
                }

                // Is there extra escape sequence?
                if (bl > 4)
                {
                    // Write string directly to output
                    if (last < curr)
                    {
                        if ((res = pOut->write(value, last, curr)) != STATUS_OK)
                            return res;
                    }

                    // Append escape sequence and reset extra buffer size
                    if ((res = pOut->write_ascii(xb, bl)) != STATUS_OK)
                        return res;
                    last = curr+1; // Put buffer pointer to the next character
                    bl = 4;
                }

                // Store previous character
                prev = ch;
            }

            // Need to emit extra data?
            if (last < curr)
            {
                if ((res = pOut->write(value, last, curr)) != STATUS_OK)
                    return res;
            }

            // Output end quote
            return pOut->write_ascii("*/", 2);
        }

        status_t Serializer::write_property(const char *name, const char *charset)
        {
            if (name == NULL)
                return STATUS_BAD_ARGUMENTS;
            LSPString tmp;
            return (tmp.set_native(name, charset)) ? write_property(&tmp) : STATUS_NO_MEM;
        }

        status_t Serializer::write_property(const char *name)
        {
            if (name == NULL)
                return STATUS_BAD_ARGUMENTS;
            LSPString tmp;
            return (tmp.set_utf8(name)) ? write_property(&tmp) : STATUS_NO_MEM;
        }

        status_t Serializer::write_property(const LSPString *name)
        {
            if (name == NULL)
                return STATUS_BAD_ARGUMENTS;
            else if (pOut == NULL)
                return STATUS_BAD_STATE;

            status_t res;

            switch (sState.mode)
            {
                case WRITE_OBJECT:
                    if (sState.flags & SF_PROPERTY)
                        return STATUS_INVALID_VALUE;
                    if ((res = emit_comma()) == STATUS_OK)
                        res = writeln();
                    if (res != STATUS_OK)
                        return res;
                    break;
                default:
                    return STATUS_BAD_STATE;
            }

            sState.flags   &= ~SF_COMMA;
            sState.flags   |= SF_PROPERTY | SF_CONTENT;

            // Do we need to prefer identifiers instead of property name?
            if ((sSettings.identifiers) && (sSettings.version >= JSON_VERSION5))
                res = (Tokenizer::is_valid_identifier(name)) ? pOut->write(name) : write_literal(name);
            else
                res = write_literal(name);

            // Write queued comment, colon and spacing
            return (res == STATUS_OK) ? pOut->write(':') : res;
        }

        status_t Serializer::start_object()
        {
            if (pOut == NULL)
                return STATUS_BAD_STATE;

            status_t res = STATUS_OK;
            switch (sState.mode)
            {
                case WRITE_ROOT:
                    if (sState.flags & SF_VALUE)
                        return STATUS_INVALID_VALUE;
                    break;
                case WRITE_ARRAY:
                    if ((res = emit_comma()) == STATUS_OK)
                        res = writeln();
                    break;
                case WRITE_OBJECT:
                    if (!(sState.flags & SF_PROPERTY))
                        return STATUS_INVALID_VALUE;
                    sState.flags &= ~SF_PROPERTY;
                    break;
                default:
                    return STATUS_BAD_STATE;
            }
            if (res == STATUS_OK)
                res = emit_separator();

            sState.flags   |= SF_VALUE | SF_CONTENT;
            if (res == STATUS_OK)
                res = pOut->write('{');
            return (res == STATUS_OK) ? push_state(WRITE_OBJECT) : res;
        }

        status_t Serializer::end_object()
        {
            if ((pOut == NULL) || (sState.mode != WRITE_OBJECT) || (sState.flags & SF_PROPERTY))
                return STATUS_BAD_STATE;
            else if ((sState.flags & SF_COMMA) && (sSettings.version < JSON_VERSION5))
                return STATUS_INVALID_VALUE;

            bool data = sState.flags & SF_VALUE;
            status_t res = pop_state();
            if ((res == STATUS_OK) && (data))
                res = writeln();

            sState.flags   &= ~SF_COMMA;
            sState.flags   |= SF_CONTENT;
            return (res == STATUS_OK) ? pOut->write('}') : res;
        }

        status_t Serializer::start_array()
        {
            if (pOut == NULL)
                return STATUS_BAD_STATE;

            status_t res = STATUS_OK;
            switch (sState.mode)
            {
                case WRITE_ROOT:
                    if (sState.flags & SF_VALUE)
                        return STATUS_INVALID_VALUE;
                    break;
                case WRITE_ARRAY:
                    if ((res = emit_comma()) == STATUS_OK)
                        res = writeln();
                    break;
                case WRITE_OBJECT:
                    if (!(sState.flags & SF_PROPERTY))
                        return STATUS_INVALID_VALUE;
                    sState.flags &= ~SF_PROPERTY;
                    break;
                default:
                    return STATUS_BAD_STATE;
            }
            if (res == STATUS_OK)
                res = emit_separator();

            sState.flags   |= SF_VALUE | SF_CONTENT;
            if (res == STATUS_OK)
                res = pOut->write('[');
            return (res == STATUS_OK) ? push_state(WRITE_ARRAY) : res;
        }

        status_t Serializer::end_array()
        {
            if ((pOut == NULL) || (sState.mode != WRITE_ARRAY))
                return STATUS_BAD_STATE;
            else if ((sState.flags & SF_COMMA) && (sSettings.version < JSON_VERSION5))
                return STATUS_INVALID_VALUE;

            bool data = sState.flags & SF_VALUE;
            status_t res = pop_state();
            if ((res == STATUS_OK) && (data))
                res = writeln();

            sState.flags   &= ~SF_COMMA;
            sState.flags   |= SF_CONTENT;
            return (res == STATUS_OK) ? pOut->write(']') : res;
        }

        status_t Serializer::prop_int(const char *key, int32_t value)
        {
            status_t res = write_property(key);
            return (res == STATUS_OK) ? write_int(value) : res;
        }

        status_t Serializer::prop_int(const LSPString *key, int32_t value)
        {
            status_t res = write_property(key);
            return (res == STATUS_OK) ? write_int(value) : res;
        }

        status_t Serializer::prop_int(const char *key, uint32_t value)
        {
            status_t res = write_property(key);
            return (res == STATUS_OK) ? write_int(value) : res;
        }

        status_t Serializer::prop_int(const LSPString *key, uint32_t value)
        {
            status_t res = write_property(key);
            return (res == STATUS_OK) ? write_int(value) : res;
        }

        status_t Serializer::prop_int(const char *key, int64_t value)
        {
            status_t res = write_property(key);
            return (res == STATUS_OK) ? write_int(value) : res;
        }

        status_t Serializer::prop_int(const LSPString *key, int64_t value)
        {
            status_t res = write_property(key);
            return (res == STATUS_OK) ? write_int(value) : res;
        }

        status_t Serializer::prop_int(const char *key, uint64_t value)
        {
            status_t res = write_property(key);
            return (res == STATUS_OK) ? write_int(value) : res;
        }

        status_t Serializer::prop_int(const LSPString *key, uint64_t value)
        {
            status_t res = write_property(key);
            return (res == STATUS_OK) ? write_int(value) : res;
        }

        status_t Serializer::prop_hex(const char *key, int32_t value)
        {
            status_t res = write_property(key);
            return (res == STATUS_OK) ? write_hex(value) : res;
        }

        status_t Serializer::prop_hex(const LSPString *key, int32_t value)
        {
            status_t res = write_property(key);
            return (res == STATUS_OK) ? write_hex(value) : res;
        }

        status_t Serializer::prop_hex(const char *key, uint32_t value)
        {
            status_t res = write_property(key);
            return (res == STATUS_OK) ? write_hex(value) : res;
        }

        status_t Serializer::prop_hex(const LSPString *key, uint32_t value)
        {
            status_t res = write_property(key);
            return (res == STATUS_OK) ? write_hex(value) : res;
        }

        status_t Serializer::prop_hex(const char *key, int64_t value)
        {
            status_t res = write_property(key);
            return (res == STATUS_OK) ? write_hex(value) : res;
        }

        status_t Serializer::prop_hex(const LSPString *key, int64_t value)
        {
            status_t res = write_property(key);
            return (res == STATUS_OK) ? write_hex(value) : res;
        }

        status_t Serializer::prop_hex(const char *key, uint64_t value)
        {
            status_t res = write_property(key);
            return (res == STATUS_OK) ? write_hex(value) : res;
        }

        status_t Serializer::prop_hex(const LSPString *key, uint64_t value)
        {
            status_t res = write_property(key);
            return (res == STATUS_OK) ? write_hex(value) : res;
        }

        status_t Serializer::prop_double(const char *key, double value)
        {
            status_t res = write_property(key);
            return (res == STATUS_OK) ? write_double(value) : res;
        }

        status_t Serializer::prop_double(const LSPString *key, double value)
        {
            status_t res = write_property(key);
            return (res == STATUS_OK) ? write_double(value) : res;
        }

        status_t Serializer::prop_double(const char *key, double value, const char *fmt)
        {
            status_t res = write_property(key);
            return (res == STATUS_OK) ? write_double(value, fmt) : res;
        }

        status_t Serializer::prop_double(const LSPString *key, double value, const char *fmt)
        {
            status_t res = write_property(key);
            return (res == STATUS_OK) ? write_double(value, fmt) : res;
        }

        status_t Serializer::prop_bool(const char *key, bool value)
        {
            status_t res = write_property(key);
            return (res == STATUS_OK) ? write_bool(value) : res;
        }

        status_t Serializer::prop_bool(const LSPString *key, bool value)
        {
            status_t res = write_property(key);
            return (res == STATUS_OK) ? write_bool(value) : res;
        }

        status_t Serializer::prop_string(const char *key, const char *value)
        {
            status_t res = write_property(key);
            return (res == STATUS_OK) ? write_string(value) : res;
        }

        status_t Serializer::prop_string(const char *key, const LSPString *value)
        {
            status_t res = write_property(key);
            return (res == STATUS_OK) ? write_string(value) : res;
        }

        status_t Serializer::prop_string(const LSPString *key, const char *value)
        {
            status_t res = write_property(key);
            return (res == STATUS_OK) ? write_string(value) : res;
        }

        status_t Serializer::prop_string(const LSPString *key, const LSPString *value)
        {
            status_t res = write_property(key);
            return (res == STATUS_OK) ? write_string(value) : res;
        }

        status_t Serializer::prop_null(const char *key)
        {
            status_t res = write_property(key);
            return (res == STATUS_OK) ? write_null() : res;
        }

        status_t Serializer::prop_null(const LSPString *key)
        {
            status_t res = write_property(key);
            return (res == STATUS_OK) ? write_null() : res;
        }

    } /* namespace json */
} /* namespace lsp */
