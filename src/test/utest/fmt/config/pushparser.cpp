/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-runtime-lib
 * Created on: 30 апр. 2020 г.
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
#include <lsp-plug.in/fmt/config/PushParser.h>
#include <lsp-plug.in/io/OutFileStream.h>
#include <stdarg.h>
#include <math.h>

namespace lsp
{
    using namespace lsp::config;

    static const char *input =
        "# comment\n"
        "\r\n"
        "key1=value1\n"
        "key2=123# comment\n"
        "key3 = 123.456\n"
        "key4 = 12.5db \n"
        "#comment\n"
        "   key5=i32:123\n"
        "\tkey6=f32:\"+12.5 dB\"\n"
        "key7=-inf\n"
        "key8=  \"+inf\"\n"
        "\n"
        "key9=blob:\":5:12345\"\n"
        "key10=blob:\"text/plain:6:123456\"\n"
        "key11=  string with spaces   # comment\n"
        "key12 = \"string \\\"with\\\" escapes\" \n"
        "key13 = string \\\"with other\\\" escapes \n"
        "key14 = string \\# not comment \n"
        "key15 = \n"
        "/tree/arg1 = str:\"value\" \n";

    static const char *output =
        "begin\n"
        "key1=str[]:value1\n"
        "key2=i32[]:123\n"
        "key3=f32[]:123.4560\n"
        "key4=f32[d]:4.2170\n"
        "key5=i32[t]:123\n"
        "key6=f32[qtd]:4.2170\n"
        "key7=f32[]:-inf\n"
        "key8=str[q]:+inf\n"
        "key9=blob[qt]:5::12345\n"
        "key10=blob[qt]:6:text/plain:123456\n"
        "key11=str[]:string with spaces\n"
        "key12=str[q]:string \"with\" escapes\n"
        "key13=str[]:string \"with other\" escapes\n"
        "key14=str[]:string # not comment\n"
        "key15=str[]:\n"
        "/tree/arg1=str[qt]:value\n"
        "end\n";
}

UTEST_BEGIN("runtime.fmt.config", pushparser)

    class Handler: public IConfigHandler
    {
        protected:
            LSPString   *pStr;
            test_type_t *pTest;

        private:
            void dump_flags(char *dst, size_t flags)
            {
                if (flags & SF_QUOTED)
                    *(dst++) = 'q';
                if (flags & SF_COMMENT)
                    *(dst++) = 'c';
                if (flags & SF_TYPE_SET)
                    *(dst++) = 't';
                if (flags & SF_DECIBELS)
                    *(dst++) = 'd';

                *dst = '\0';
            }

        public:
            Handler(test_type_t *t, LSPString *str) { pStr = str; pTest = t; }

        public:
            virtual status_t start()
            {
                pStr->append_ascii("begin\n");
                pTest->printf("begin\n");
                return STATUS_OK;
            }

            virtual status_t finish(status_t res)
            {
                pStr->append_ascii("end\n");
                pTest->printf("end\n");
                return STATUS_OK;
            }

            virtual status_t handle_i32(const LSPString *name, int32_t value, size_t flags)
            {
                char f[0x10];
                dump_flags(f, flags);
                pStr->fmt_append_utf8("%s=i32[%s]:%d\n", name->get_utf8(), f, int(value));
                pTest->printf("%s=i32[%s]:%d\n", name->get_utf8(), f, int(value));
                return STATUS_OK;
            }

            virtual status_t handle_u32(const LSPString *name, uint32_t value, size_t flags)
            {
                char f[0x10];
                dump_flags(f, flags);
                pStr->fmt_append_utf8("%s=u32[%s]:%d\n", name->get_utf8(), f, int(value));
                pTest->printf("%s=u32[%s]:%d\n", name->get_utf8(), f, int(value));
                return STATUS_OK;
            }

            virtual status_t handle_f32(const LSPString *name, float value, size_t flags)
            {
                char f[0x10];
                dump_flags(f, flags);
                pStr->fmt_append_utf8("%s=f32[%s]:%.4f\n", name->get_utf8(), f, value);
                pTest->printf("%s=f32[%s]:%.4f\n", name->get_utf8(), f, value);
                return STATUS_OK;
            }

            virtual status_t handle_i64(const LSPString *name, int64_t value, size_t flags)
            {
                char f[0x10];
                dump_flags(f, flags);
                pStr->fmt_append_utf8("%s=i64[%s]:%d\n", name->get_utf8(), f, int(value));
                pTest->printf("%s=i64[%s]:%d\n", name->get_utf8(), f, int(value));
                return STATUS_OK;
            }

            virtual status_t handle_u64(const LSPString *name, uint64_t value, size_t flags)
            {
                char f[0x10];
                dump_flags(f, flags);
                pStr->fmt_append_utf8("%s=u64[%s]:%d\n", name->get_utf8(), f, int(value));
                pTest->printf("%s=u64[%s]:%d\n", name->get_utf8(), f, int(value));
                return STATUS_OK;
            }

            virtual status_t handle_f64(const LSPString *name, double value, size_t flags)
            {
                char f[0x10];
                dump_flags(f, flags);
                pStr->fmt_append_utf8("%s=f64[%s]:%.4f\n", name->get_utf8(), f, value);
                pTest->printf("%s=f64[%s]:%.4f\n", name->get_utf8(), f, value);
                return STATUS_OK;
            }

            virtual status_t handle_string(const LSPString *name, const LSPString *value, size_t flags)
            {
                char f[0x10];
                dump_flags(f, flags);
                pStr->fmt_append_utf8("%s=str[%s]:%s\n", name->get_utf8(), f, value->get_utf8());
                pTest->printf("%s=str[%s]:%s\n", name->get_utf8(), f, value->get_utf8());
                return STATUS_OK;
            }

            virtual status_t handle_blob(const LSPString *name, const blob_t *value, size_t flags)
            {
                char f[0x10];
                dump_flags(f, flags);
                pStr->fmt_append_utf8("%s=blob[%s]:%d:%s:%s\n", name->get_utf8(), f,
                        int(value->length),
                        (value->ctype != NULL) ? value->ctype : "",
                        (value->data != NULL) ? value->data : ""
                );
                pTest->printf("%s=blob[%s]:%d:%s:%s\n", name->get_utf8(), f,
                        int(value->length),
                        (value->ctype != NULL) ? value->ctype : "",
                        (value->data != NULL) ? value->data : ""
                );
                return STATUS_OK;
            }
    };

    void test_valid_cases()
    {
        PushParser p;
        LSPString out;
        Handler h(this, &out);

        UTEST_ASSERT(p.parse_data(&h, input, "UTF-8") == STATUS_OK);
        UTEST_ASSERT(out.equals_utf8(output),
                printf("Got output:\n%s\n", out.get_utf8());
                printf("Expected output:\n%s\n", output);
            );
    }

    void test_file()
    {
        PushParser p;
        LSPString out;
        Handler h(this, &out);
        io::Path path;
        UTEST_ASSERT(path.fmt("%s/%s.obj", tempdir(), full_name()) >= 0);

        printf("Writing simple config data to %s...\n", path.as_native());
        io::OutFileStream os;
        UTEST_ASSERT(os.open(&path, io::File::FM_WRITE_NEW) == STATUS_OK);
        UTEST_ASSERT(os.write(input, ::strlen(input)) > 0);
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
//        printf("Testing simple data...\n");
//        test_valid_cases();
        printf("Testing file parse...\n");
        test_file();
    }

UTEST_END



