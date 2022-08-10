/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-runtime-lib
 * Created on: 14 окт. 2019 г.
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

#include <lsp-plug.in/fmt/url.h>
#include <lsp-plug.in/fmt/bookmarks.h>
#include <lsp-plug.in/fmt/json/Parser.h>
#include <lsp-plug.in/fmt/json/Serializer.h>
#include <lsp-plug.in/fmt/xml/PushParser.h>
#include <lsp-plug.in/io/InFileStream.h>
#include <lsp-plug.in/io/InSequence.h>
#include <lsp-plug.in/io/OutMemoryStream.h>

#ifdef PLATFORM_WINDOWS
    #define FS_CHAR_MAIN    '\\'
    #define FS_CHAR_ALT     '/'
#else
    #define FS_CHAR_MAIN    '/'
    #define FS_CHAR_ALT     '\\'
#endif /* PLATFORM_WINDOWS */

namespace lsp
{
    namespace bookmarks
    {
        class XbelParser: public xml::IXMLHandler
        {
            private:
                lltl::parray<bookmark_t>   *vList;
                size_t                      nOrigin;
                bookmark_t                 *pCurr;
                bool                        bTitle;
                LSPString                   sPath;

            protected:
                static void flush_os(LSPString *dst, io::OutMemoryStream *os)
                {
                    if (os->size() <= 0)
                        return;
                    dst->append_utf8(reinterpret_cast<const char *>(os->data()), os->size());
                    os->clear();
                }

                static int hexdigit(lsp_wchar_t ch)
                {
                    if ((ch >= '0') && (ch <= '9'))
                        return ch - '0';
                    if ((ch >= 'A') && (ch <= 'F'))
                        return ch - 'A' + 10;
                    if ((ch >= 'a') && (ch <= 'f'))
                        return ch - 'f' + 10;
                    return -1;
                }

                static void get_bookmark_name(LSPString *dst, const LSPString *href)
                {
                    ssize_t idx = lsp_max(href->rindex_of(FS_CHAR_MAIN), href->rindex_of(FS_CHAR_ALT));
                    idx         = lsp_max(0, idx + 1);

                    io::OutMemoryStream os;
                    lsp_finally { os.close(); };

                    for (ssize_t len = href->length(); idx < len; ++idx)
                    {
                        lsp_wchar_t ch = href->at(idx);

                        // Special case?
                        if (ch == '%')
                        {
                            int d1 = hexdigit(href->at(idx+1));
                            int d2 = hexdigit(href->at(idx+2));
                            if ((d1 >= 0) && (d2 >= 0))
                            {
                                os.writeb((d1 << 4) | d2);
                                idx += 2;
                                continue;
                            }
                        }

                        flush_os(dst, &os);
                        dst->append(ch);
                    }

                    flush_os(dst, &os);
                }


            public:
                explicit XbelParser(lltl::parray<bookmark_t> *list, size_t origin)
                {
                    vList       = list;
                    nOrigin     = origin;
                    pCurr       = NULL;
                    bTitle      = false;
                }

            public:
                virtual status_t doctype(const LSPString *doctype, const LSPString *pub, const LSPString *sys)
                {
                    if ((doctype != NULL) && (!doctype->equals_ascii_nocase("xbel")))
                        return STATUS_BAD_FORMAT;
                    return STATUS_OK;
                }

                virtual status_t start_element(const LSPString *name, const LSPString * const *atts)
                {
                    // Append tag name to path
                    if (!sPath.append('/'))
                        return STATUS_NO_MEM;
                    if (!sPath.append(name))
                        return STATUS_NO_MEM;

                    // Check path
                    if (sPath.equals_ascii("/xbel/bookmark"))
                    {
                        LSPString href;

                        // Scan for 'href' attribute
                        for ( ; *atts != NULL; atts += 2)
                            if (atts[0]->equals_ascii("href"))
                            {
                                if (atts[1]->starts_with_ascii("file://"))
                                {
                                    if (!href.set(atts[1], 7))
                                        return STATUS_NO_MEM;
                                }
                                break;
                            }

                        // Do we have a reference?
                        if (href.length() > 0)
                        {
                            // Allocate bookmark descriptor
                            bookmark_t *bm  = new bookmark_t();
                            if (bm == NULL)
                                return STATUS_NO_MEM;
                            if (!vList->add(bm))
                            {
                                delete bm;
                                return STATUS_NO_MEM;
                            }

                            // Initialize bookmark
                            get_bookmark_name(&bm->name, &href);
                            bm->origin      = BM_LSP | nOrigin;
                            bm->path.swap(&href);

                            // Save pointer to bookmark
                            pCurr           = bm;
                            bTitle          = false;
                        }
                    }

                    return STATUS_OK;
                }

                virtual status_t characters(const LSPString *text)
                {
                    if (sPath.equals_ascii("/xbel/bookmark/title"))
                    {
                        if (pCurr == NULL)
                            return STATUS_OK;

                        bool success    = (bTitle) ? pCurr->name.append(text) : pCurr->name.set(text);
                        if (!success)
                            return STATUS_NO_MEM;
                        bTitle          = true;
                    }
                    return STATUS_OK;
                }

                virtual status_t end_element(const LSPString *name)
                {
                    // Forget about current bookmark
                    if (sPath.equals_ascii("/xbel/bookmark"))
                    {
                        pCurr       = NULL;
                        bTitle      = false;
                    }

                    // Reduce path
                    ssize_t idx = sPath.rindex_of('/');
                    sPath.set_length((idx >= 0) ? idx : 0);
                    return STATUS_OK;
                }
        };

        void destroy_bookmarks(lltl::parray<bookmark_t> *list)
        {
            if (list == NULL)
                return;

            for (size_t i=0, n=list->size(); i<n; ++i)
            {
                bookmark_t *ptr = list->uget(i);
                if (ptr != NULL)
                    delete ptr;
            }
            list->flush();
        }

        bool bookmark_exists(const lltl::parray<bookmark_t> *search, const LSPString *path)
        {
            for (size_t i=0, n=search->size(); i<n; ++i)
            {
                const bookmark_t *bm = search->uget(i);
                if ((bm != NULL) && (bm->path.equals(path)))
                    return true;
            }
            return false;
        }

        status_t merge_bookmarks(lltl::parray<bookmark_t> *dst, size_t *changes, const lltl::parray<bookmark_t> *src, bm_origin_t origin)
        {
            if ((dst == NULL) || (src == NULL) || (origin == 0))
                return STATUS_BAD_ARGUMENTS;

            size_t nc = 0;

            // Step 1: check presence of all bookmarks of 'dst' in 'src'
            for (size_t i=0; i<dst->size(); )
            {
                bookmark_t *bm = dst->uget(i);
                if (bm == NULL) // Remove all NULL entries
                {
                    size_t sz = dst->size();
                    if (dst->remove(i))
                        ++nc;
                    else if (dst->size() == sz)
                        return STATUS_NO_MEM;
                    continue;
                }
                else
                    ++i;

                if (bookmark_exists(src, &bm->path))
                {
                    if (!(bm->origin & origin))
                    {
                        bm->origin     |= origin;
                        ++nc;
                    }
                }
                else if (bm->origin & origin)
                {
                    bm->origin     &= ~origin;
                    ++nc;
                }
            }

            // Step 2: check presence of all bookmarks of 'src' in 'dst'
            for (size_t i=0, n=src->size(); i<n; ++i)
            {
                const bookmark_t *bm = src->uget(i);
                if ((bm == NULL) || (bookmark_exists(dst, &bm->path)))
                    continue;

                // Copy bookmark
                bookmark_t *dm = new bookmark_t;
                if (dm == NULL)
                    return STATUS_NO_MEM;

                if ((!dm->path.set(&bm->path)) ||
                    (!dm->name.set(&bm->name)))
                {
                    delete dm;
                    return STATUS_NO_MEM;
                }

                if (!dst->add(dm))
                {
                    delete dm;
                    return STATUS_NO_MEM;
                }

                // Mark with origin + LSP flag
                dm->origin      = (origin | BM_LSP);
                ++nc;
            }

            // Step 3: remove all bookmarks with empty flags
            for (size_t i=0; i<dst->size();)
            {
                bookmark_t *bm = dst->uget(i);
                if (bm->origin == 0)
                {
                    if (!dst->remove(i))
                        return STATUS_NO_MEM;
                    delete bm;
                    ++nc;
                    continue;
                }
                else
                    ++i;
            }

            // Store number of changes
            if (changes != NULL)
                *changes   += nc;

            return STATUS_OK;
        }

        //---------------------------------------------------------------------
        // GTK3 stuff
        status_t read_bookmarks_gtk(lltl::parray<bookmark_t> *dst, io::IInSequence *in, size_t origin)
        {
            lltl::parray<bookmark_t> vtmp;
            LSPString tmp;
            status_t res;
            ssize_t split;

            while (true)
            {
                // Read line
                res = in->read_line(&tmp, true);
                if (res != STATUS_OK)
                {
                    if (res == STATUS_EOF)
                        break;
                    destroy_bookmarks(&vtmp);
                    return res;
                }

                // Analyze line
                if (!tmp.starts_with_ascii_nocase("file://"))
                    continue;


                // Create bookmark
                bookmark_t *bm  = new bookmark_t;
                bm->origin      = origin;
                if (bm == NULL)
                {
                    destroy_bookmarks(&vtmp);
                    return STATUS_NO_MEM;
                }

                // Is there alias?
                split = tmp.index_of(' ');

                // Decode bookmark respectively to the presence of alias
                if ((res = url::decode(&bm->path, &tmp, 7, (split < 0) ? tmp.length() : split)) != STATUS_OK)
                {
                    delete bm;
                    destroy_bookmarks(&vtmp);
                    return STATUS_NO_MEM;
                }

                // Obtain the last name in path
                if (split < 0)
                {
                    split = lsp_max(bm->path.rindex_of(FS_CHAR_MAIN), split = bm->path.rindex_of(FS_CHAR_ALT));
                    if (split < 0)
                        split = -1;
                    if (!bm->name.set(&bm->path, split + 1))
                    {
                        delete bm;
                        destroy_bookmarks(&vtmp);
                        return STATUS_NO_MEM;
                    }
                }
                else if (!bm->name.set(&tmp, split + 1))
                {
                    delete bm;
                    destroy_bookmarks(&vtmp);
                    return STATUS_NO_MEM;
                }

                // Add to list
                if (!vtmp.add(bm))
                {
                    delete bm;
                    destroy_bookmarks(&vtmp);
                    return STATUS_NO_MEM;
                }
            }

            dst->swap(&vtmp);
            destroy_bookmarks(&vtmp);

            return STATUS_OK;
        }

        status_t read_bookmarks_gtk(lltl::parray<bookmark_t> *dst, const char *path, const char *charset, size_t origin)
        {
            if ((path == NULL) || (dst == NULL))
                return STATUS_BAD_ARGUMENTS;

            lltl::parray<bookmark_t> tmp;
            io::InSequence is;
            status_t res;
            if ((res = is.open(path, charset)) == STATUS_OK)
            {
                res = read_bookmarks_gtk(&tmp, &is, origin);
                if (res == STATUS_OK)
                    res = is.close();
                else
                    is.close();
            }

            if (res == STATUS_OK)
                dst->swap(&tmp);
            destroy_bookmarks(&tmp);

            return res;
        }

        status_t read_bookmarks_gtk(lltl::parray<bookmark_t> *dst, const LSPString *path, const char *charset, size_t origin)
        {
            if ((path == NULL) || (dst == NULL))
                return STATUS_BAD_ARGUMENTS;

            lltl::parray<bookmark_t> tmp;
            io::InSequence is;
            status_t res;
            if ((res = is.open(path, charset)) == STATUS_OK)
            {
                res = read_bookmarks_gtk(&tmp, &is, origin);
                if (res == STATUS_OK)
                    res = is.close();
                else
                    is.close();
            }

            if (res == STATUS_OK)
                dst->swap(&tmp);
            destroy_bookmarks(&tmp);

            return res;
        }

        status_t read_bookmarks_gtk(lltl::parray<bookmark_t> *dst, const io::Path *path, const char *charset, size_t origin)
        {
            if ((path == NULL) || (dst == NULL))
                return STATUS_BAD_ARGUMENTS;

            lltl::parray<bookmark_t> tmp;
            io::InSequence is;
            status_t res;
            if ((res = is.open(path, charset)) == STATUS_OK)
            {
                res = read_bookmarks_gtk(&tmp, &is, origin);
                if (res == STATUS_OK)
                    res = is.close();
                else
                    is.close();
            }

            if (res == STATUS_OK)
                dst->swap(&tmp);
            destroy_bookmarks(&tmp);

            return res;
        }

        status_t read_bookmarks_gtk2(lltl::parray<bookmark_t> *dst, const char *path, const char *charset)
        {
            return read_bookmarks_gtk(dst, path, charset, BM_GTK2);
        }

        status_t read_bookmarks_gtk2(lltl::parray<bookmark_t> *dst, const LSPString *path, const char *charset)
        {
            return read_bookmarks_gtk(dst, path, charset, BM_GTK2);
        }

        status_t read_bookmarks_gtk2(lltl::parray<bookmark_t> *dst, const io::Path *path, const char *charset)
        {
            return read_bookmarks_gtk(dst, path, charset, BM_GTK2);
        }

        status_t read_bookmarks_gtk2(lltl::parray<bookmark_t> *dst, io::IInSequence *in)
        {
            return read_bookmarks_gtk(dst, in, BM_GTK2);
        }

        status_t read_bookmarks_gtk3(lltl::parray<bookmark_t> *dst, const char *path, const char *charset)
        {
            return read_bookmarks_gtk(dst, path, charset, BM_GTK3);
        }

        status_t read_bookmarks_gtk3(lltl::parray<bookmark_t> *dst, const LSPString *path, const char *charset)
        {
            return read_bookmarks_gtk(dst, path, charset, BM_GTK3);
        }

        status_t read_bookmarks_gtk3(lltl::parray<bookmark_t> *dst, const io::Path *path, const char *charset)
        {
            return read_bookmarks_gtk(dst, path, charset, BM_GTK3);
        }

        status_t read_bookmarks_gtk3(lltl::parray<bookmark_t> *dst, io::IInSequence *in)
        {
            return read_bookmarks_gtk(dst, in, BM_GTK3);
        }

        //---------------------------------------------------------------------
        // QT5 stuff
        //---------------------------------------------------------------------

        status_t read_bookmarks_qt5(lltl::parray<bookmark_t> *dst, const char *path, const char *charset)
        {
            lltl::parray<bookmark_t> tmp;
            status_t res;

            xml::PushParser p;
            XbelParser h(&tmp, BM_QT5);
            if ((res = p.parse_file(&h, path, charset)) == STATUS_OK)
                dst->swap(&tmp);

            destroy_bookmarks(&tmp);
            return res;
        }

        status_t read_bookmarks_qt5(lltl::parray<bookmark_t> *dst, const LSPString *path, const char *charset)
        {
            lltl::parray<bookmark_t> tmp;
            status_t res;

            xml::PushParser p;
            XbelParser h(&tmp, BM_QT5);
            if ((res = p.parse_file(&h, path, charset)) == STATUS_OK)
                dst->swap(&tmp);

            destroy_bookmarks(&tmp);
            return res;
        }

        status_t read_bookmarks_qt5(lltl::parray<bookmark_t> *dst, const io::Path *path, const char *charset)
        {
            lltl::parray<bookmark_t> tmp;
            status_t res;

            xml::PushParser p;
            XbelParser h(&tmp, BM_QT5);
            if ((res = p.parse_file(&h, path, charset)) == STATUS_OK)
                dst->swap(&tmp);

            destroy_bookmarks(&tmp);
            return res;
        }

        status_t read_bookmarks_qt5(lltl::parray<bookmark_t> *dst, io::IInSequence *in)
        {
            lltl::parray<bookmark_t> tmp;
            status_t res;

            xml::PushParser p;
            XbelParser h(&tmp, BM_QT5);
            if ((res = p.parse_data(&h, in)) == STATUS_OK)
                dst->swap(&tmp);

            destroy_bookmarks(&tmp);
            return res;
        }

        //---------------------------------------------------------------------
        // LSP stuff
        status_t read_json_origin(size_t *origin, json::Parser &p)
        {
            json::event_t ev;

            status_t res = p.read_next(&ev);
            if (res != STATUS_OK)
                return res;
            if (ev.type != json::JE_ARRAY_START)
                return STATUS_CORRUPTED;

            while (true)
            {
                // Check that it is not end of array
                status_t res = p.read_next(&ev);
                if (res != STATUS_OK)
                    return res;
                if (ev.type == json::JE_ARRAY_END)
                    break;
                else if (ev.type != json::JE_STRING)
                    return STATUS_CORRUPTED;

                // Analyze string
                if (ev.sValue.equals_ascii("lsp"))
                    *origin    |= BM_LSP;
                else if (ev.sValue.equals_ascii("gtk2"))
                    *origin    |= BM_GTK2;
                else if (ev.sValue.equals_ascii("gtk3"))
                    *origin    |= BM_GTK3;
                else if (ev.sValue.equals_ascii("qt5"))
                    *origin    |= BM_QT5;
            }

            return STATUS_OK;
        }

        status_t read_json_item(bookmark_t *item, json::Parser &p)
        {
            json::event_t ev;

            while (true)
            {
                // Check that it is not end of array
                status_t res = p.read_next(&ev);
                if (res != STATUS_OK)
                    return res;
                if (ev.type == json::JE_OBJECT_END)
                    break;
                else if (ev.type != json::JE_PROPERTY)
                    return STATUS_CORRUPTED;

                // Read properties
                if (ev.sValue.equals_ascii("path"))
                {
                    if ((res = p.read_string(&item->path)) != STATUS_OK)
                        return res;
                }
                else if (ev.sValue.equals_ascii("name"))
                {
                    if ((res = p.read_string(&item->name)) != STATUS_OK)
                        return res;
                }
                else if (ev.sValue.equals_ascii("origin"))
                {
                    if ((res = read_json_origin(&item->origin, p)) != STATUS_OK)
                        return res;
                }
                else if ((res = p.skip_next()) != STATUS_OK)
                    return res;
            }

            return STATUS_OK;
        }

        status_t read_json_bookmarks(lltl::parray<bookmark_t> *dst, json::Parser &p)
        {
            json::event_t ev;

            status_t res = p.read_next(&ev);
            if (res != STATUS_OK)
                return res;
            if (ev.type != json::JE_ARRAY_START)
                return STATUS_CORRUPTED;

            while (true)
            {
                // Check that it is not end of array
                status_t res = p.read_next(&ev);
                if (res != STATUS_OK)
                    return res;
                if (ev.type == json::JE_ARRAY_END)
                    break;
                else if (ev.type != json::JE_OBJECT_START)
                    return STATUS_CORRUPTED;

                // Read bookmark item
                bookmark_t *item = new bookmark_t;
                item->origin     = 0;

                res = read_json_item(item, p);
                if (res != STATUS_OK)
                {
                    if (res == STATUS_NULL)
                        res = STATUS_CORRUPTED;
                    delete item;
                    return res;
                }
                else if (!dst->add(item))
                {
                    delete item;
                    return STATUS_NO_MEM;
                }
            }

            return res;
        }

        status_t read_bookmarks(lltl::parray<bookmark_t> *dst, json::Parser &p)
        {
            lltl::parray<bookmark_t> tmp;
            status_t res = read_json_bookmarks(&tmp, p);
            if (res == STATUS_OK)
                res = p.close();

            if (res != STATUS_OK)
                p.close();
            else
                dst->swap(&tmp);
            destroy_bookmarks(&tmp);

            return res;
        }

        status_t read_bookmarks(lltl::parray<bookmark_t> *dst, const char *path, const char *charset)
        {
            if (dst == NULL)
                return STATUS_BAD_ARGUMENTS;

            json::Parser p;
            status_t res = p.open(path, json::JSON_VERSION5, charset);
            return (res == STATUS_OK) ? read_bookmarks(dst, p) : res;
        }

        status_t read_bookmarks(lltl::parray<bookmark_t> *dst, const LSPString *path, const char *charset)
        {
            if (dst == NULL)
                return STATUS_BAD_ARGUMENTS;

            json::Parser p;
            status_t res = p.open(path, json::JSON_VERSION5, charset);
            return (res == STATUS_OK) ? read_bookmarks(dst, p) : res;
        }

        status_t read_bookmarks(lltl::parray<bookmark_t> *dst, const io::Path *path, const char *charset)
        {
            if (dst == NULL)
                return STATUS_BAD_ARGUMENTS;

            json::Parser p;
            status_t res = p.open(path, json::JSON_VERSION5, charset);
            return (res == STATUS_OK) ? read_bookmarks(dst, p) : res;
        }

        status_t read_bookmarks(lltl::parray<bookmark_t> *dst, io::IInSequence *in)
        {
            if (dst == NULL)
                return STATUS_BAD_ARGUMENTS;

            json::Parser p;
            lltl::parray<bookmark_t> tmp;
            status_t res = p.wrap(in, json::JSON_VERSION5, WRAP_NONE);
            return (res == STATUS_OK) ? read_bookmarks(dst, p) : res;
        }

        void init_settings(json::serial_flags_t *s)
        {
            s->version      = json::JSON_VERSION5;
            s->identifiers  = false;
            s->ident        = ' ';
            s->padding      = 4;
            s->separator    = true;
            s->multiline    = true;
        }

        status_t save_item(const bookmark_t *item, json::Serializer &s)
        {
            status_t res;
            if ((res = s.start_object()) != STATUS_OK)
                return res;
            {
                // Path
                if ((res = s.write_property("path")) != STATUS_OK)
                    return res;
                if ((res = s.write_string(&item->path)) != STATUS_OK)
                    return res;

                // Name
                if ((res = s.write_property("name")) != STATUS_OK)
                    return res;
                if ((res = s.write_string(&item->name)) != STATUS_OK)
                    return res;

                // Origin
                if ((res = s.write_property("origin")) != STATUS_OK)
                    return res;
                bool ml = s.set_multiline(false);
                if ((res = s.start_array()) != STATUS_OK)
                    return res;
                {
                    if ((item->origin & BM_LSP) && ((res = s.write_string("lsp")) != STATUS_OK))
                        return res;
                    if ((item->origin & BM_GTK2) && ((res = s.write_string("gtk2")) != STATUS_OK))
                        return res;
                    if ((item->origin & BM_GTK3) && ((res = s.write_string("gtk3")) != STATUS_OK))
                        return res;
                    if ((item->origin & BM_QT5) && ((res = s.write_string("qt5")) != STATUS_OK))
                        return res;
                }
                if ((res = s.end_array()) != STATUS_OK)
                    return res;
                s.set_multiline(ml);
            }

            if ((res = s.end_object()) != STATUS_OK)
                return res;

            return res;
        }

        status_t save_bookmarks(const lltl::parray<bookmark_t> *src, json::Serializer &s)
        {
            static const char *comment = "\n"
                    " * This file contains list of bookmarked directories.\n"
                    " * \n"
                    " ";

            // Write header
            status_t res = s.write_comment(comment);
            if (res == STATUS_OK)
                res = s.writeln();
            if (res == STATUS_OK)
                res = s.start_array();

            // Serialize body
            if (res == STATUS_OK)
            {
                for (size_t i=0, n=src->size(); i<n; ++i)
                {
                    const bookmark_t *bm = src->uget(i);
                    if ((bm == NULL) || (bm->origin == 0)) // Skip bookmarks with empty origin
                        continue;
                    if ((res = save_item(bm, s)) != STATUS_OK)
                        break;
                }
            }

            if (res == STATUS_OK)
                res = s.end_array();

            // Close serializer
            if (res == STATUS_OK)
                res = s.close();
            else
                s.close();

            return res;
        }

        status_t save_bookmarks(const lltl::parray<bookmark_t> *src, const char *path, const char *charset)
        {
            json::Serializer s;
            json::serial_flags_t flags;
            init_settings(&flags);
            status_t res = s.open(path, &flags, charset);
            return (res == STATUS_OK) ? save_bookmarks(src, s) : res;
        }

        status_t save_bookmarks(const lltl::parray<bookmark_t> *src, const LSPString *path, const char *charset)
        {
            json::Serializer s;
            json::serial_flags_t flags;
            init_settings(&flags);
            status_t res = s.open(path, &flags, charset);
            return (res == STATUS_OK) ? save_bookmarks(src, s) : res;
        }

        status_t save_bookmarks(const lltl::parray<bookmark_t> *src, const io::Path *path, const char *charset)
        {
            json::Serializer s;
            json::serial_flags_t flags;
            init_settings(&flags);
            status_t res = s.open(path, &flags, charset);
            return (res == STATUS_OK) ? save_bookmarks(src, s) : res;
        }

        status_t save_bookmarks(const lltl::parray<bookmark_t> *src, io::IOutSequence *out)
        {
            json::Serializer s;
            json::serial_flags_t flags;
            init_settings(&flags);
            status_t res = s.wrap(out, &flags, WRAP_NONE);
            return (res == STATUS_OK) ? save_bookmarks(src, s) : res;
        }


    }
}



