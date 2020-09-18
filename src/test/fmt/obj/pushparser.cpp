/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-runtime-lib
 * Created on: 23 апр. 2020 г.
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
#include <lsp-plug.in/fmt/obj/PushParser.h>
#include <lsp-plug.in/io/OutFileStream.h>
#include <stdarg.h>

namespace lsp
{
    using namespace lsp::obj;

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

    static const char *output =
        "BOLS Triangle 1\n"
        "V -2 -1 -1 1\n"
        "V 2 -1 -1 1\n"
        "V 0 2 -1 1\n"
        "VN 0 0 1 0\n"
        "F 0/0/-1 1/0/-1 2/0/-1\n"
        "EO\n"
        "BOLS Triangle 2\n"
        "V -2 -1 -2 1\n"
        "V 2 -1 -2 1\n"
        "V 0 2 -2 1\n"
        "VT -2 -1 0\n"
        "VT 2 -1 0\n"
        "VT 0 2 0\n"
        "VN 0 0 1 0\n"
        "VN 0 0 1 0\n"
        "F 3/1/0 4/1/1 5/2/2\n"
        "F 5/2/2 4/2/1 3/1/0\n"
        "EO\n"
        "EOD\n";
}

UTEST_BEGIN("runtime.fmt.obj", pushparser)

    class Handler: public IObjHandler
    {
        protected:
            LSPString &data;

        public:
            explicit Handler(LSPString &out) : data(out) {}

            virtual status_t begin_object(const char *name)
            {
                data.fmt_append_utf8("BOCC %s\n", name);
                return STATUS_OK;
            }

            virtual status_t begin_object(const LSPString *name)
            {
                data.fmt_append_utf8("BOLS %s\n", name->get_utf8());
                return STATUS_OK;
            }

            virtual status_t end_object()
            {
                data.fmt_append_utf8("EO\n");
                return STATUS_OK;
            }

            virtual status_t end_of_data()
            {
                data.fmt_append_utf8("EOD\n");
                return STATUS_OK;
            }

            virtual ssize_t add_vertex(float x, float y, float z, float w)
            {
                data.fmt_append_utf8("V %d %d %d %d\n", int(x), int(y), int(z), int(w));
                return STATUS_OK;
            }

            virtual ssize_t add_param_vertex(float x, float y, float z, float w)
            {
                data.fmt_append_utf8("VP %d %d %d %d\n", int(x), int(y), int(z), int(w));
                return STATUS_OK;
            }

            virtual ssize_t add_normal(float nx, float ny, float nz, float nw)
            {
                data.fmt_append_utf8("VN %d %d %d %d\n", int(nx), int(ny), int(nz), int(nw));
                return STATUS_OK;
            }

            virtual ssize_t add_texture_vertex(float u, float v, float w)
            {
                data.fmt_append_utf8("VT %d %d %d\n", int(u), int(v), int(w));
                return STATUS_OK;
            }

            virtual status_t add_face(const index_t *vv, const index_t *vn, const index_t *vt, size_t n)
            {
                data.append_ascii("F");
                for (size_t i=0; i<n; ++i)
                    data.fmt_append_utf8(" %d/%d/%d", vv[i], vn[i], vt[i]);
                data.append('\n');

                return STATUS_OK;
            }

            virtual status_t add_points(const index_t *vv, size_t n)
            {
                data.append_ascii("P");
                for (size_t i=0; i<n; ++i)
                    data.fmt_append_utf8(" %d", vv[i]);
                data.append('\n');

                return STATUS_OK;
            }

            virtual status_t add_line(const index_t *vv, const index_t *vt, size_t n)
            {
                data.append_ascii("L");
                for (size_t i=0; i<n; ++i)
                    data.fmt_append_utf8(" %d/%d", vv[i], vt[i]);
                data.append('\n');

                return STATUS_OK;
            }
    };


    void test_simple_obj_data()
    {
        PushParser p;
        LSPString out;
        Handler h(out);

        printf("Testing simple OBJ data...\n");
        UTEST_ASSERT(p.parse_data(&h, contents) == STATUS_OK);
        UTEST_ASSERT(out.equals_utf8(output),
                printf("Got output:\n%s\n", out.get_utf8());
                printf("Expected output:\n%s\n", output);
            );
    }

    void test_load_obj_data()
    {
        PushParser p;
        LSPString out;
        Handler h(out);
        io::Path path;
        UTEST_ASSERT(path.fmt("%s/%s.obj", tempdir(), full_name()) >= 0);

        printf("Writing simple OBJ data to %s...\n", path.as_native());
        io::OutFileStream os;
        UTEST_ASSERT(os.open(&path, io::File::FM_WRITE_NEW) == STATUS_OK);
        UTEST_ASSERT(os.write(contents, strlen(contents)) > 0);
        UTEST_ASSERT(os.close() == STATUS_OK);

        printf("Parsing simple OBJ data from %s...\n", path.as_native());

        printf("Testing simple OBJ data...\n");
        UTEST_ASSERT(p.parse_file(&h, &path) == STATUS_OK);
        UTEST_ASSERT(out.equals_utf8(output),
                printf("Got output:\n%s\n", out.get_utf8());
                printf("Expected output:\n%s\n", output);
            );
    }

    UTEST_MAIN
    {
        test_simple_obj_data();
        test_load_obj_data();
    }

UTEST_END


