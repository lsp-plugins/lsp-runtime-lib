/*
 * Copyright (C) 2021 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2021 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-runtime-lib
 * Created on: 15 февр. 2021 г.
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

#include <lsp-plug.in/fmt/json/dom.h>
#include <lsp-plug.in/fmt/json/Parser.h>

namespace lsp
{
    namespace json
    {
        status_t dom_parse(Parser *p, Node *node, bool strict)
        {
            // TODO
            return STATUS_NOT_IMPLEMENTED;
        }

        status_t dom_load(const char *path, Node *node, json_version_t version, const char *charset)
        {
            Parser p;
            Node tmp;
            status_t res = p.open(path, version, charset);
            if (res == STATUS_OK)
                res     = dom_parse(&p, &tmp, true);

            if (res == STATUS_OK)
                res = p.close();
            else
                p.close();

            if (res == STATUS_OK)
                node->assign(tmp);

            return res;
        }

        status_t dom_load(const LSPString *path, Node *node, json_version_t version, const char *charset)
        {
            Parser p;
            Node tmp;
            status_t res = p.open(path, version, charset);
            if (res == STATUS_OK)
                res     = dom_parse(&p, &tmp, true);

            if (res == STATUS_OK)
                res = p.close();
            else
                p.close();

            if (res == STATUS_OK)
                node->assign(tmp);

            return res;
        }

        status_t dom_load(const io::Path *path, Node *node, json_version_t version, const char *charset)
        {
            Parser p;
            Node tmp;
            status_t res = p.open(path, version, charset);
            if (res == STATUS_OK)
                res     = dom_parse(&p, &tmp, true);

            if (res == STATUS_OK)
                res = p.close();
            else
                p.close();

            if (res == STATUS_OK)
                node->assign(tmp);

            return res;
        }

        size_t build_wflags(size_t flags)
        {
            size_t wflags = 0;
            if (flags & DOM_CLOSE)
                wflags     |= WRAP_CLOSE;
            if (flags & DOM_DELETE)
                wflags     |= WRAP_DELETE;
            return wflags;
        }

        status_t dom_parse(io::IInStream *is, Node *node, json_version_t version, size_t flags, const char *charset)
        {
            Parser p;
            Node tmp;

            size_t wflags   = build_wflags(flags);
            status_t res    = p.wrap(is, version, wflags, charset);
            if (res == STATUS_OK)
                res = dom_parse(&p, &tmp, !(flags & DOM_FLEXIBLE));

            if (res == STATUS_OK)
                res = p.close();
            else
                p.close();

            if (res == STATUS_OK)
                node->assign(tmp);

            return res;
        }

        status_t dom_parse(const LSPString *data, Node *node, json_version_t version)
        {
            Parser p;
            Node tmp;

            status_t res    = p.wrap(data, version);
            if (res == STATUS_OK)
                res = dom_parse(&p, &tmp, true);

            if (res == STATUS_OK)
                res = p.close();
            else
                p.close();

            if (res == STATUS_OK)
                node->assign(tmp);

            return res;
        }

        status_t dom_parse(io::IInSequence *is, Node *node, json_version_t version, size_t flags)
        {
            Parser p;
            Node tmp;

            size_t wflags   = build_wflags(flags);
            status_t res    = p.wrap(is, version, wflags);
            if (res == STATUS_OK)
                res = dom_parse(&p, &tmp, !(flags & DOM_FLEXIBLE));

            if (res == STATUS_OK)
                res = p.close();
            else
                p.close();

            if (res == STATUS_OK)
                node->assign(tmp);

            return res;
        }
    }
}
