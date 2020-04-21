/*
 * PullParser.cpp
 *
 *  Created on: 21 апр. 2017 г.
 *      Author: sadko
 */

#include <lsp-plug.in/fmt/obj/PullParser.h>

#include <lsp-plug.in/io/InFileStream.h>
#include <lsp-plug.in/io/InStringSequence.h>
#include <lsp-plug.in/io/InSequence.h>

namespace lsp
{
    namespace obj
    {
        PullParser::PullParser()
        {
            pIn         = NULL;
            nWFlags     = 0;
            pBuffer     = NULL;
            nBufOff     = 0;
            nBufLen     = 0;
            bSkipLF     = false;
        }

        PullParser::~PullParser()
        {
            close();
        }

        status_t PullParser::open(const char *path, const char *charset)
        {
            if (pIn != NULL)
                return STATUS_BAD_STATE;
            else if (path == NULL)
                return STATUS_BAD_ARGUMENTS;

            io::InFileStream *ifs = new io::InFileStream();
            if (ifs == NULL)
                return STATUS_NO_MEM;
            status_t res = ifs->open(path);
            if (res == STATUS_OK)
            {
                res     = wrap(ifs, WRAP_CLOSE | WRAP_DELETE, charset);
                if (res == STATUS_OK)
                    return res;
                ifs->close();
            }
            delete ifs;

            return res;
        }

        status_t PullParser::open(const LSPString *path, const char *charset)
        {
            if (pIn != NULL)
                return STATUS_BAD_STATE;
            else if (path == NULL)
                return STATUS_BAD_ARGUMENTS;

            io::InFileStream *ifs = new io::InFileStream();
            if (ifs == NULL)
                return STATUS_NO_MEM;
            status_t res = ifs->open(path);
            if (res == STATUS_OK)
            {
                res     = wrap(ifs, WRAP_CLOSE | WRAP_DELETE, charset);
                if (res == STATUS_OK)
                    return res;
                ifs->close();
            }
            delete ifs;

            return res;
        }

        status_t PullParser::open(const io::Path *path, const char *charset)
        {
            if (pIn != NULL)
                return STATUS_BAD_STATE;
            else if (path == NULL)
                return STATUS_BAD_ARGUMENTS;

            io::InFileStream *ifs = new io::InFileStream();
            if (ifs == NULL)
                return STATUS_NO_MEM;
            status_t res = ifs->open(path);
            if (res == STATUS_OK)
            {
                res     = wrap(ifs, WRAP_CLOSE | WRAP_DELETE, charset);
                if (res == STATUS_OK)
                    return res;
                ifs->close();
            }
            delete ifs;

            return res;
        }

        status_t PullParser::wrap(const char *str, const char *charset)
        {
            if (pIn != NULL)
                return STATUS_BAD_STATE;
            else if (str == NULL)
                return STATUS_BAD_ARGUMENTS;

            io::InStringSequence *seq = new io::InStringSequence();
            if (seq == NULL)
                return STATUS_NO_MEM;

            status_t res = seq->wrap(str, charset);
            if (res == STATUS_OK)
            {
                if ((res = wrap(seq, WRAP_CLOSE | WRAP_DELETE)) == STATUS_OK)
                    return res;
                seq->close();
            }

            delete seq;
            return res;
        }

        status_t PullParser::wrap(const LSPString *str)
        {
            if (pIn != NULL)
                return STATUS_BAD_STATE;
            else if (str == NULL)
                return STATUS_BAD_ARGUMENTS;

            io::InStringSequence *seq = new io::InStringSequence();
            if (seq == NULL)
                return STATUS_NO_MEM;

            status_t res = seq->wrap(str);
            if (res == STATUS_OK)
            {
                if ((res = wrap(seq, WRAP_CLOSE | WRAP_DELETE)) == STATUS_OK)
                    return res;
                seq->close();
            }

            delete seq;
            return res;
        }

        status_t PullParser::wrap(io::IInStream *is, size_t flags, const char *charset)
        {
            if (pIn != NULL)
                return STATUS_BAD_STATE;
            else if (is == NULL)
                return STATUS_BAD_ARGUMENTS;

            io::InSequence *seq = new io::InSequence();
            if (seq == NULL)
                return STATUS_NO_MEM;

            status_t res = seq->wrap(is, flags, charset);
            if (res == STATUS_OK)
            {
                if ((res = wrap(seq, WRAP_CLOSE | WRAP_DELETE)) == STATUS_OK)
                    return res;
                seq->close();
            }

            delete seq;
            return res;
        }

        status_t PullParser::wrap(io::IInSequence *seq, size_t flags)
        {
            if (pIn != NULL)
                return STATUS_BAD_STATE;
            else if (seq == NULL)
                return STATUS_BAD_ARGUMENTS;

            // Allocate memory
            lsp_wchar_t *b  = static_cast<lsp_wchar_t *>(::malloc(IO_BUF_SIZE * sizeof(lsp_wchar_t)));
            if (b == NULL)
                return STATUS_NO_MEM;

            // Initialize state
            pBuffer         = b;
            nBufOff         = 0;
            nBufLen         = 0;
            bSkipLF         = false;
            pIn             = seq;

            return STATUS_OK;
        }

        status_t PullParser::close()
        {
            status_t res = STATUS_OK;

            // Reset internal state
            if (pBuffer != NULL)
            {
                ::free(pBuffer);
                pBuffer = NULL;
            }
            nBufOff         = 0;
            nBufLen         = 0;
            bSkipLF         = false;

            // Release input sequence
            if (pIn != NULL)
            {
                if (nWFlags & WRAP_CLOSE)
                {
                    if (res == STATUS_OK)
                        res = pIn->close();
                    else
                        pIn->close();
                }

                if (nWFlags & WRAP_DELETE)
                    delete pIn;

                pIn     = NULL;
            }

            return res;
        }

        const event_t *PullParser::current() const
        {
            return (sEvent.type != event_type_t(-1)) ? &sEvent : NULL;
        };

        status_t PullParser::next(event_t *ev)
        {
            status_t res = read_event();
            if ((res == STATUS_OK) && (ev != NULL))
                res = copy_event(ev);
            return res;
        }

        status_t PullParser::copy_event(event_t *ev)
        {
            // Set values to target structure
            ev->type    = sEvent.type;
            ev->vertex  = sEvent.vertex;

            if (!ev->name.set(&sEvent.name))
                return STATUS_NO_MEM;
            if (!ev->ivertex.set(&sEvent.ivertex))
                return STATUS_NO_MEM;
            if (!ev->inormal.set(&sEvent.inormal))
                return STATUS_NO_MEM;
            if (!ev->itexcoord.set(&sEvent.itexcoord))
                return STATUS_NO_MEM;

            return STATUS_OK;
        }

        status_t PullParser::read_event()
        {
            status_t res = STATUS_OK;

            while (true)
            {
                // Clear current event
                sEvent.type     = event_type_t(-1);
                sEvent.name.clear();
                sEvent.ivertex.clear();
                sEvent.inormal.clear();
                sEvent.itexcoord.clear();

                // read line
                if ((res = read_line()) != STATUS_OK)
                    return res;
            }

            // TODO
            return STATUS_OK;
        }

        status_t PullParser::read_line()
        {
            // TODO
            return STATUS_NOT_IMPLEMENTED;
        }

        /*inline bool Parser::is_space(char ch)
        {
            return (ch == ' ') || (ch == '\t');
        }

        inline bool Parser::prefix_match(const char *s, const char *prefix)
        {
            while (*prefix != '\0')
            {
                if (*(s++) != *(prefix++))
                    return false;
            }
            return is_space(*s);
        }

        bool Parser::parse_float(float *dst, const char **s)
        {
            if (*s == NULL)
                return false;

            errno = 0;
            char *ptr = NULL;
            float result = strtof(*s, &ptr);
            if ((errno != 0) || (ptr == *s))
                return false;
            *dst    = result;
            *s      = ptr;
            return true;
        }

        bool Parser::parse_int(ssize_t *dst, const char **s)
        {
            if ((*s == NULL) || (**s == '\0') || (**s == ' '))
                return false;

            errno = 0;
            char *ptr = NULL;
            long result = strtol(*s, &ptr, 10);
            if ((errno != 0) || (ptr == *s))
                return false;
            *dst    = result;
            *s      = ptr;
            return true;
        }

        const char *Parser::skip_spaces(const char *s)
        {
            if (s == NULL)
                return NULL;

            while (true)
            {
                char ch = *s;
                if ((ch == '\0') || (!is_space(ch)))
                    return s;
                s++;
            }
        }

        bool Parser::end_of_line(const char *s)
        {
            if (s == NULL)
                return true;

            while (true)
            {
                char ch = *s;
                if (ch == '\0')
                    return true;
                if (!is_space(ch))
                    return false;
                s++;
            }
        }

        void Parser::eliminate_comments(LSPString *s)
        {
            size_t len = s->length(), r=0, w=0;
            bool slash = false;

            while (r < len)
            {
                lsp_wchar_t ch = s->char_at(r);
                if (slash)
                {
                    ++r;
                    if ((ch != '#') && (ch != '\\'))
                        s->set_at(w++, '\\');
                    s->set_at(w++, ch);
                    slash   = false;
                    continue;
                }
                else if (ch == '#')
                {
                    s->set_length(r);
                    return;
                }
                else if (ch == '\\')
                {
                    slash = true;
                    ++r;
                    continue;
                }

                if (r != w)
                    s->set_at(w, ch);
                ++r, ++w;
            }

            if (slash)
                s->set_at(w++, '\\');
            s->set_length(w);
        }

        status_t Parser::read_line(file_buffer_t *fb)
        {
            // Clear previous line contents
            fb->line.clear();

            while (true)
            {
                // Ensure that there is data in buffer
                if (fb->off >= fb->len)
                {
                    // No data in the buffer, read from input stream
                    ssize_t n = fb->in->read(fb->data, IO_BUF_SIZE);
                    if (n <= 0)
                    {
                        if (n != -STATUS_EOF)
                            return -n;
                        return (fb->line.length() > 0) ? STATUS_OK : STATUS_EOF;
                    }
                    fb->len     = n;
                    fb->off     = 0;
                }

                // Scan for line ending
                if (fb->skip_wc)
                {
                    fb->skip_wc = false;
                    if (fb->data[fb->off] == '\r')
                    {
                        ++fb->off;
                        continue;
                    }
                }

                // Scan for line ending character
                size_t tail = fb->off;
                while (tail < fb->len)
                {
                    lsp_wchar_t ch = fb->data[tail++];
                    if (ch == '\n') // Found!
                    {
                        fb->skip_wc = true;
                        break;
                    }
                }

                // Append data to string and update buffer state
                fb->line.append(&fb->data[fb->off], tail - fb->off);
                fb->off = tail;

                // Now analyze last string character
                size_t len = fb->line.length();
                if (fb->line.last() != '\n') // Not end of line?
                    continue;
                fb->line.set_length(--len);

                // Compute number of terminating '\\' characters
                ssize_t slashes = 0, xoff = len-1;
                while ((xoff >= 0) && (fb->line.char_at(xoff) == '\\'))
                {
                    ++slashes;
                    --xoff;
                }

                // Line has been split into multiple lines?
                if (slashes & 1)
                {
                    fb->line.set_length(--len);
                    continue;
                }

                // Alright, now we have complete line and can return it
                eliminate_comments(&fb->line);
                return STATUS_OK;
            }
        }

        status_t Parser::parse_lines(file_buffer_t *fb, IObjHandler *handler)
        {
            status_t result = STATUS_OK;

            parse_state_t state;
            state.pHandler      = handler;
            state.nObjectID     = -1;
            state.nPointID      = 0;
            state.nFaceID       = 0;
            state.nLineID       = 0;
            state.nLines        = 0;

            while (true)
            {
                // Try to read line
                result = read_line(fb);
                if (result != STATUS_OK)
                {
                    if (result == STATUS_EOF)
                        result      = parse_finish(&state);
                    break;
                }

                // Check that line is not empty
                const char *l = skip_spaces(fb->line.get_utf8());
                if ((l == NULL) || (*l == '\0'))
                    continue;

                // Parse line
                result = parse_line(&state, l);
                if (result != STATUS_OK)
                    break;
            }

            // Destroy state
            state.sVx.flush();
            state.sParVx.flush();
            state.sTexVx.flush();
            state.sNorm.flush();

            state.sVxIdx.flush();
            state.sTexVxIdx.flush();
            state.sNormIdx.flush();

            return result;
        }

        status_t Parser::parse(const char *path, IObjHandler *handler)
        {
            if ((path == NULL) || (handler == NULL))
                return STATUS_BAD_ARGUMENTS;

            LSPString spath;
            if (!spath.set_utf8(path))
                return STATUS_NO_MEM;

            return parse(&spath, handler);
        }

        status_t Parser::parse(const LSPString *path, IObjHandler *handler)
        {
            if ((path == NULL) || (handler == NULL))
                return STATUS_BAD_ARGUMENTS;

            io::InSequence in;
            status_t res = in.open(path, "UTF-8");
            if (res != STATUS_OK)
                return res;

            // Initialize file buffer
            file_buffer_t fb;
            fb.in       = &in;
            fb.len      = 0;
            fb.off      = 0;
            fb.skip_wc  = false;
            fb.data     = reinterpret_cast<lsp_wchar_t *>(::malloc(IO_BUF_SIZE * sizeof(lsp_wchar_t)));
            if (fb.data == NULL)
            {
                in.close();
                return STATUS_NO_MEM;
            }

            char *saved_locale = setlocale(LC_NUMERIC, "C");
            status_t result     = parse_lines(&fb, handler);
            setlocale(LC_NUMERIC, saved_locale);

            // Destroy buffer data
            ::free(fb.data);
            in.close();

            return result;
        }

        status_t Parser::parse(const io::Path *path, IObjHandler *handler)
        {
            if ((path == NULL) || (handler == NULL))
                return STATUS_BAD_ARGUMENTS;
            return parse(path->as_string(), handler);
        }

        status_t Parser::parse_line(parse_state_t *st, const char *s)
        {
    //        lsp_trace("%s", s);
            status_t result = ((st->nLines++) > 0) ? STATUS_CORRUPTED_FILE : STATUS_BAD_FORMAT;

            switch (*(s++))
            {
                case 'b': // bmat, bevel
                    if (prefix_match(s, "mat")) // bmat
                        return STATUS_OK;
                    else if (prefix_match(s, "evel")) // bevel
                        return STATUS_OK;
                    break;

                case 'c': // cstype, curv, curv2, con, c_interp, ctech
                    if (prefix_match(s, "stype")) // cstype
                        return STATUS_OK;
                    else if (prefix_match(s, "urv")) // curv
                        return STATUS_OK;
                    else if (prefix_match(s, "urv2")) // curv2
                        return STATUS_OK;
                    else if (prefix_match(s, "on")) // con
                        return STATUS_OK;
                    else if (prefix_match(s, "_interp")) // c_interp
                        return STATUS_OK;
                    else if (prefix_match(s, "tech")) // ctech
                        return STATUS_OK;
                    break;

                case 'd': // deg, d_interp
                    if (prefix_match(s, "eg")) // deg
                        return STATUS_OK;
                    else if (prefix_match(s, "_interp")) // d_interp
                        return STATUS_OK;
                    break;

                case 'e': // end
                    if (prefix_match(s, "nd")) // end
                        return STATUS_OK;
                    break;

                case 'f': // f
                    if (is_space(*s)) // f - face
                    {
                        // Clear previously used lists
                        st->sVxIdx.clear();
                        st->sTexVxIdx.clear();
                        st->sNormIdx.clear();

                        // Parse face
                        while (true)
                        {
                            ssize_t v = 0, vt = 0, vn = 0;

                            // Parse indexes
                            s   = skip_spaces(s);
                            if (!parse_int(&v, &s))
                                break;
                            if (*(s++) != '/')
                                return result;
                            if (!parse_int(&vt, &s))
                                vt  = 0;
                            if (*(s++) != '/')
                                return result;
                            if (!parse_int(&vn, &s))
                                vn = 0;

                            // Ensure that indexes are correct
                            v   = (v < 0) ? st->sVx.size() + v : v - 1;
                            if ((v < 0) || (v >= ssize_t(st->sVx.size())))
                                return result;

                            vt  = (vt < 0) ? st->sTexVx.size() + vt : vt - 1;
                            if ((vt < -1) || (vt >= ssize_t(st->sTexVx.size())))
                                return result;

                            vn  = (vn < 0) ? st->sNorm.size() + vn : vn - 1;
                            if ((vn < -1) || (vn >= ssize_t(st->sNorm.size())))
                                return result;

                            // Register vertex
                            ofp_point3d_t *xp = st->sVx.at(v);
                            if (xp->oid != st->nObjectID)
                            {
                                xp->oid     = st->nObjectID;
                                xp->idx     = st->pHandler->add_vertex(xp);
                                if (xp->idx < 0)
                                    return -xp->idx;
                            }
                            v           = xp->idx;

                            // Register texture vertex
                            if (vt >= 0)
                            {
                                xp = st->sTexVx.at(vt);
                                if (xp->oid != st->nObjectID)
                                {
                                    xp->oid     = st->nObjectID;
                                    xp->idx     = st->pHandler->add_texture_vertex(xp);
                                    if (xp->idx < 0)
                                        return -xp->idx;
                                }
                                vt  = xp->idx;
                            }

                            // Register normal vector
                            if (vn >= 0)
                            {
                                ofp_vector3d_t *xn = st->sNorm.at(vn);
                                if (xn == NULL)
                                    return STATUS_BAD_FORMAT;
                                vn  = xn->idx;
                            }

                            // Add items to lists
                            if (!st->sVxIdx.add(v))
                                return STATUS_NO_MEM;
                            if (!st->sTexVxIdx.add(vt))
                                return STATUS_NO_MEM;
                            if (!st->sNormIdx.add(vn))
                                return STATUS_NO_MEM;
                        }

                        if (!end_of_line(s))
                            return result;

                        // Check face parameters
                        if (st->sVxIdx.size() < 3)
                            return STATUS_BAD_FORMAT;

                        // Call parser to handle data
                        result = st->pHandler->add_face(st->sVxIdx.get_array(), st->sNormIdx.get_array(), st->sTexVxIdx.get_array(), st->sVxIdx.size());
                    }
                    break;

                case 'g': // g
                    if (is_space(*s)) // g
                        return STATUS_OK;
                    break;

                case 'h': // hole
                    if (prefix_match(s, "ole")) // hole
                        return STATUS_OK;
                    break;

                case 'l': // l, lod
                    if (is_space(*s)) // l - line
                    {
                        // Clear previously used lists
                        st->sVxIdx.clear();
                        st->sTexVxIdx.clear();

                        // Parse face
                        while (true)
                        {
                            ssize_t v = 0, vt = 0;

                            // Parse indexes
                            s   = skip_spaces(s);
                            if (!parse_int(&v, &s))
                                break;
                            if (*(s++) != '/')
                                return result;
                            if (!parse_int(&vt, &s))
                                vt = 0;

                            // Ensure that indexes are correct
                            v   = (v < 0) ? st->sVx.size() + v : v - 1;
                            if ((v < 0) || (v >= ssize_t(st->sVx.size())))
                                return result;

                            vt  = (vt < 0) ? st->sTexVx.size() + vt : vt - 1;
                            if ((vt <= -1) || (vt >= ssize_t(st->sTexVx.size())))
                                return result;

                            // Register vertex
                            ofp_point3d_t *xp   = st->sVx.at(v);
                            if (xp->oid != st->nObjectID)
                            {
                                xp->oid     = st->nObjectID;
                                xp->idx     = st->pHandler->add_vertex(xp);
                                if (xp->idx < 0)
                                    return -xp->idx;
                            }
                            v           = xp->idx;

                            // Register texture vertex
                            if (vt >= 0)
                            {
                                xp = st->sTexVx.at(vt);
                                if (xp->oid != st->nObjectID)
                                {
                                    xp->oid     = st->nObjectID;
                                    xp->idx     = st->pHandler->add_texture_vertex(xp);
                                    if (xp->idx < 0)
                                        return -xp->idx;
                                }
                                vt  = xp->idx;
                            }

                            // Add items to lists
                            if (!st->sVxIdx.add(&v))
                                return STATUS_NO_MEM;
                            if (!st->sTexVxIdx.add(&vt))
                                return STATUS_NO_MEM;
                        }

                        if (!end_of_line(s))
                            return result;

                        // Check line parameters
                        if (st->sVxIdx.size() < 2)
                            return STATUS_BAD_FORMAT;

                        // Call parser to handle data
                        result = st->pHandler->add_line(st->sVxIdx.get_array(), st->sTexVxIdx.get_array(), st->sVxIdx.size());
                    }
                    else if (prefix_match(s, "od")) // lod
                        return STATUS_OK;
                    break;

                case 'm': // mg, mtllib
                    if (prefix_match(s, "g")) // mg
                        return STATUS_OK;
                    else if (prefix_match(s, "tllib")) // mtllib
                        return STATUS_OK;
                    break;

                case 'o': // o
                    if (is_space(*s)) // o
                    {
                        s   = skip_spaces(s+1);
                        if (st->nObjectID >= 0)
                        {
                            result = st->pHandler->end_object(st->nObjectID);
                            if (result != STATUS_OK)
                                return result;
                        }
                        result = st->pHandler->begin_object(++st->nObjectID, s);
                    }
                    break;

                case 'p': // p, parm
                    if (is_space(*s)) // p
                    {
                        st->sVxIdx.clear();

                        // Parse point
                        while (true)
                        {
                            ssize_t v = 0;

                            // Parse indexes
                            s   = skip_spaces(s);
                            if (!parse_int(&v, &s))
                                break;

                            // Ensure that indexes are correct
                            v   = (v < 0) ? st->sVx.size() + v : v - 1;
                            if ((v < 0) || (v >= ssize_t(st->sVx.size())))
                                return result;

                            // Register vertex
                            ofp_point3d_t *xp   = st->sVx.at(v);
                            if (xp->oid != st->nObjectID)
                            {
                                xp->oid     = st->nObjectID;
                                xp->idx     = st->pHandler->add_vertex(xp);
                                if (xp->idx < 0)
                                    return -xp->idx;
                            }
                            v           = xp->idx;

                            // Add items to lists
                            if (!st->sVxIdx.add(&v))
                                return STATUS_NO_MEM;
                        }

                        // Check that we reached end of line
                        if (!end_of_line(s))
                            return result;

                        result = st->pHandler->add_points(st->sVxIdx.get_array(), st->sVxIdx.size());
                    }
                    else if (prefix_match(s, "arm")) // parm
                        return STATUS_OK;
                    break;

                case 's': // s, step, surf, scrv, sp, shadow_obj, stech
                    if (is_space(*s)) // s
                        return STATUS_OK;
                    else if (prefix_match(s, "tep")) // step
                        return STATUS_OK;
                    else if (prefix_match(s, "urf")) // surf
                        return STATUS_OK;
                    else if (prefix_match(s, "rcv")) // srcv
                        return STATUS_OK;
                    else if (prefix_match(s, "p")) // sp
                        return STATUS_OK;
                    else if (prefix_match(s, "hadow_obj")) // shadow_obj
                        return STATUS_OK;
                    else if (prefix_match(s, "tech")) // stech
                        return STATUS_OK;
                    break;

                case 't': // trim, trace_obj
                    if (prefix_match(s, "rim")) // trim
                        return STATUS_OK;
                    else if (prefix_match(s, "race_obj")) // trace_obj
                        return STATUS_OK;
                    break;

                case 'u': // usemtl
                    if (prefix_match(s, "semtl")) // usemtl
                        return STATUS_OK;
                    break;

                case 'v': // v, vt, vn, vp
                    if (is_space(*s)) // v
                    {
                        ofp_point3d_t p;

                        s   = skip_spaces(s+1);
                        if (!parse_float(&p.x, &s))
                            return result;
                        s   = skip_spaces(s);
                        if (!parse_float(&p.y, &s))
                            return result;
                        s   = skip_spaces(s);
                        if (!parse_float(&p.z, &s))
                            p.z     = 0.0f; // Extension, strictly required in obj format, for our case facilitated
                        s   = skip_spaces(s);
                        if (!parse_float(&p.w, &s))
                            p.w     = 1.0f;

                        if (!end_of_line(s))
                            return result;

                        p.oid       = -1;
                        p.idx       = -1;
                        if (!st->sVx.add(&p))
                            return STATUS_NO_MEM;
                        result = STATUS_OK;
                    }
                    else if (prefix_match(s, "n")) // vn
                    {
                        ofp_vector3d_t v;

                        s   = skip_spaces(s+2);
                        if (!parse_float(&v.dx, &s))
                            return result;
                        s   = skip_spaces(s);
                        if (!parse_float(&v.dy, &s))
                            return result;
                        s   = skip_spaces(s);
                        if (!parse_float(&v.dz, &s))
                            v.dz    = 0.0f; // Extension, strictly required in obj format, for our case facilitated
                        v.dw    = 0.0f;

                        if (!end_of_line(s))
                            return result;

                        v.oid       = -1;
                        v.idx       = st->pHandler->add_normal(&v);
                        if (v.idx < 0)
                            return -v.idx;
                        if (!st->sNorm.add(&v))
                            return STATUS_NO_MEM;
                        result = STATUS_OK;
                    }
                    else if (prefix_match(s, "p")) // vp
                    {
                        ofp_point3d_t p;

                        s   = skip_spaces(s+2);
                        if (parse_float(&p.x, &s))
                            return result;
                        s   = skip_spaces(s);
                        if (!parse_float(&p.y, &s))
                            p.y     = 0.0f;
                        p.z     = 0.0f;
                        s   = skip_spaces(s);
                        if (!parse_float(&p.w, &s))
                            p.w     = 1.0f;

                        if (!end_of_line(s))
                            return result;

                        p.oid       = -1;
                        p.idx       = -1;
                        if (!st->sParVx.add(&p))
                            return STATUS_NO_MEM;
                        result = STATUS_OK;
                    }
                    else if (prefix_match(s, "t")) // vt
                    {
                        ofp_point3d_t p;

                        s   = skip_spaces(s+2);
                        if (!parse_float(&p.x, &s))
                            return result;
                        s   = skip_spaces(s);
                        if (!parse_float(&p.y, &s))
                            p.y     = 0.0f;
                        p.z     = 0.0f;
                        s   = skip_spaces(s);
                        if (!parse_float(&p.w, &s))
                            p.w     = 0.0f;

                        if (!end_of_line(s))
                            return result;

                        p.oid       = -1;
                        p.idx       = -1;
                        if (!st->sTexVx.add(&p))
                            return STATUS_NO_MEM;
                        result = STATUS_OK;
                    }
                    break;
            }

            return result;
        }

        status_t Parser::parse_finish(parse_state_t *st)
        {
            status_t result = STATUS_OK;

            if (st->nObjectID >= 0)
            {
                result = st->pHandler->end_object(st->nObjectID);
                if (result != STATUS_OK)
                    return result;
            }

            if (result == STATUS_OK)
                result = st->pHandler->end_of_data();

            return result;
        }*/
    }
} /* namespace lsp */
