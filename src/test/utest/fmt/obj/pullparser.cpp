/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-runtime-lib
 * Created on: 22 апр. 2020 г.
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

#include <lsp-plug.in/test-fw/utest.h>
#include <lsp-plug.in/test-fw/helpers.h>
#include <lsp-plug.in/fmt/obj/PullParser.h>
#include <stdarg.h>

namespace lsp
{
    using namespace lsp::obj;
}

UTEST_BEGIN("runtime.fmt.obj", pullparser)
    bool ck_v(const event_t *ev, float x, float y, float z, float w)
    {
        if (ev == NULL)
            return false;
        return
            float_equals_absolute(x, ev->vertex.x) &&
            float_equals_absolute(y, ev->vertex.y) &&
            float_equals_absolute(z, ev->vertex.z) &&
            float_equals_absolute(w, ev->vertex.w);
    }

    bool ck_vn(const event_t *ev, float dx, float dy, float dz, float dw)
    {
        if (ev == NULL)
            return false;
        return
            float_equals_absolute(dx, ev->normal.dx) &&
            float_equals_absolute(dy, ev->normal.dy) &&
            float_equals_absolute(dz, ev->normal.dz) &&
            float_equals_absolute(dw, ev->normal.dw);
    }

    bool ck_vt(const event_t *ev, float u, float v, float w)
    {
        if (ev == NULL)
            return false;
        return
            float_equals_absolute(u, ev->texcoord.u) &&
            float_equals_absolute(v, ev->texcoord.v) &&
            float_equals_absolute(w, ev->texcoord.w);
    }

    bool ck_iv(const lltl::darray<index_t> *xv, size_t n ...)
    {
        if ((xv == NULL) || (xv->size() != n))
            return false;

        va_list v;
        va_start(v, n);

        for (size_t i=0; i<n; ++i)
        {
            int idx = va_arg(v, int);
            const index_t *v = xv->uget(i);
            if (*v != idx)
                return false;
        }

        va_end(v);

        return true;
    }

    void test_simple_obj_file()
    {
        static const char *contents=
            "# Some comment\n"
            "o Triangle 1\n"
            "v -2 -1 -1 #comment\n"
            "v 2 -1 -1\n"
            "v 0 2 -1\n"
            "vn 0 0 1\n"
            "f 1//1 2//1 3//1\n"
            "\n"
            "o   Triangle 2\n\r"
            "v -2.0 -1.0 -2.0\n\r"
            "v 2.0 -1.0 -2.0\n\r"
            "v 0.0 2.0 -2.0\n\r"
            "vt -2.0 -1.0\n\r"
            "vt 2.0 -1.0\n\r"
            "vt 0.0 2.0\n\r"
            "vn 0.0 0.0 1.0\n\r"
            "vn 0.1 0.1 1.0\n\r"
            "f 4/1/2 5/2/2 6/3/3\n\r"
            "f -1/-1/-1 -2/-2/-1 -3/-3/-2\n\r"
            "\n\r"
            "\n\r";

        PullParser p;
        event_t ev;
        const event_t *pev;

        UTEST_ASSERT(p.current() == NULL);
        UTEST_ASSERT(p.current(&ev) == STATUS_CLOSED);
        UTEST_ASSERT(p.wrap(contents, "UTF-8") == STATUS_OK);
        UTEST_ASSERT(p.current() == NULL);
        UTEST_ASSERT(p.current(&ev) == STATUS_NO_DATA);

        // Object 1
        UTEST_ASSERT(p.next() == STATUS_OK);
            UTEST_ASSERT((pev = p.current()) != NULL);
            UTEST_ASSERT(pev->type == EV_OBJECT);
            UTEST_ASSERT(pev->name.equals_ascii("Triangle 1"));

        UTEST_ASSERT(p.next() == STATUS_OK);
            UTEST_ASSERT((pev = p.current()) != NULL);
            UTEST_ASSERT(pev->type == EV_VERTEX);
            UTEST_ASSERT(ck_v(pev, -2.0f, -1.0f, -1.0f, 1.0f));
        UTEST_ASSERT(p.next() == STATUS_OK);
            UTEST_ASSERT((pev = p.current()) != NULL);
            UTEST_ASSERT(pev->type == EV_VERTEX);
            UTEST_ASSERT(ck_v(pev, 2.0f, -1.0f, -1.0f, 1.0f));
        UTEST_ASSERT(p.next() == STATUS_OK);
            UTEST_ASSERT((pev = p.current()) != NULL);
            UTEST_ASSERT(pev->type == EV_VERTEX);
            UTEST_ASSERT(ck_v(pev, 0.0f, 2.0f, -1.0f, 1.0f));

        UTEST_ASSERT(p.next() == STATUS_OK);
            UTEST_ASSERT((pev = p.current()) != NULL);
            UTEST_ASSERT(pev->type == EV_NORMAL);
            UTEST_ASSERT(ck_vn(pev, 0.0f, 0.0f, 1.0f, 0.0f));

        UTEST_ASSERT(p.next() == STATUS_OK);
            UTEST_ASSERT((pev = p.current()) != NULL);
            UTEST_ASSERT(pev->type == EV_FACE);
            UTEST_ASSERT(ck_iv(&pev->ivertex, 3, 0, 1, 2));
            UTEST_ASSERT(ck_iv(&pev->itexcoord, 3, -1, -1, -1));
            UTEST_ASSERT(ck_iv(&pev->inormal, 3, 0, 0, 0));

        UTEST_ASSERT(p.current(&ev) == STATUS_OK);
            UTEST_ASSERT(ev.type == EV_FACE);
            UTEST_ASSERT(ck_iv(&ev.ivertex, 3, 0, 1, 2));
            UTEST_ASSERT(ck_iv(&ev.itexcoord, 3, -1, -1, -1));
            UTEST_ASSERT(ck_iv(&ev.inormal, 3, 0, 0, 0));

        // Object 2
        UTEST_ASSERT(p.next(&ev) == STATUS_OK);
            UTEST_ASSERT(ev.type == EV_OBJECT);
            UTEST_ASSERT(ev.name.equals_ascii("Triangle 2"));

        UTEST_ASSERT(p.next(&ev) == STATUS_OK);
            UTEST_ASSERT(ev.type == EV_VERTEX);
            UTEST_ASSERT(ck_v(&ev, -2.0f, -1.0f, -2.0f, 1.0f));
        UTEST_ASSERT(p.next(&ev) == STATUS_OK);
            UTEST_ASSERT(ev.type == EV_VERTEX);
            UTEST_ASSERT(ck_v(&ev, 2.0f, -1.0f, -2.0f, 1.0f));
        UTEST_ASSERT(p.next(&ev) == STATUS_OK);
            UTEST_ASSERT(ev.type == EV_VERTEX);
            UTEST_ASSERT(ck_v(&ev, 0.0f, 2.0f, -2.0f, 1.0f));

        UTEST_ASSERT(p.next(&ev) == STATUS_OK);
            UTEST_ASSERT(ev.type == EV_TEXCOORD);
            UTEST_ASSERT(ck_vt(&ev, -2.0f, -1.0f, 0.0f));
        UTEST_ASSERT(p.next(&ev) == STATUS_OK);
            UTEST_ASSERT(ev.type == EV_TEXCOORD);
            UTEST_ASSERT(ck_vt(&ev, 2.0f, -1.0f, 0.0f));
        UTEST_ASSERT(p.next(&ev) == STATUS_OK);
            UTEST_ASSERT(ev.type == EV_TEXCOORD);
            UTEST_ASSERT(ck_vt(&ev, 0.0f, 2.0f, 0.0f));

        UTEST_ASSERT(p.next(&ev) == STATUS_OK);
            UTEST_ASSERT(ev.type == EV_NORMAL);
            UTEST_ASSERT(ck_vt(&ev, 0.0f, 0.0f, 1.0f));

        UTEST_ASSERT(p.next(&ev) == STATUS_OK);
            UTEST_ASSERT(ev.type == EV_NORMAL);
            UTEST_ASSERT(ck_vt(&ev, 0.1f, 0.1f, 1.0f));

        UTEST_ASSERT(p.next(&ev) == STATUS_OK);
            UTEST_ASSERT(ev.type == EV_FACE);
            UTEST_ASSERT(ck_iv(&ev.ivertex, 3, 3, 4, 5));
            UTEST_ASSERT(ck_iv(&ev.itexcoord, 3, 0, 1, 2));
            UTEST_ASSERT(ck_iv(&ev.inormal, 3, 1, 1, 2));

        UTEST_ASSERT(p.next(&ev) == STATUS_OK);
            UTEST_ASSERT(ev.type == EV_FACE);
            UTEST_ASSERT(ck_iv(&ev.ivertex, 3, 5, 4, 3));
            UTEST_ASSERT(ck_iv(&ev.itexcoord, 3, 2, 1, 0));
            UTEST_ASSERT(ck_iv(&ev.inormal, 3, 2, 2, 1));

        // End of file
        UTEST_ASSERT(p.next() == STATUS_EOF);
        UTEST_ASSERT(p.next() == STATUS_EOF);

        UTEST_ASSERT(p.current() == NULL);
        UTEST_ASSERT(p.current(&ev) == STATUS_NO_DATA);

        // Close file
        UTEST_ASSERT(p.close() == STATUS_OK);
        UTEST_ASSERT(p.current() == NULL);
        UTEST_ASSERT(p.current(&ev) == STATUS_CLOSED);
    }

    void test_load_obj_file(const char *fname)
    {
        io::Path path;
        UTEST_ASSERT(path.fmt("%s/%s", resources(), fname) > 0);

        printf("Reading file %s\n", path.as_native());
        PullParser p;
        status_t res;

        UTEST_ASSERT(p.open(&path) == STATUS_OK);

        while ((res = p.next()) == STATUS_OK)
        {
            const char *type = NULL;
            UTEST_ASSERT(p.current() != NULL);
            switch (p.current()->type)
            {
                case EV_VERTEX: type = "vertex"; break;
                case EV_NORMAL: type = "normal"; break;
                case EV_FACE: type = "face"; break;
                case EV_OBJECT: type = "object"; break;
                default: UTEST_FAIL_MSG("Unknown event type: %d", int(p.current()->type));
            }
            printf(" read event: %s\n", type);
        }

        UTEST_ASSERT(res == STATUS_EOF);
        UTEST_ASSERT(p.close() == STATUS_OK);
    }

    UTEST_MAIN
    {
        printf("Testing simple OBJ file...\n");
        test_simple_obj_file();

        printf("Testing load of complicated OBJ file...\n");
        test_load_obj_file("fmt/obj/swimming-pool.obj");
    }

UTEST_END


