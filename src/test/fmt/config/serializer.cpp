/*
 * serializer.cpp
 *
 *  Created on: 2 мая 2020 г.
 *      Author: sadko
 */

#include <lsp-plug.in/test-fw/utest.h>
#include <lsp-plug.in/test-fw/helpers.h>
#include <lsp-plug.in/fmt/config/PullParser.h>
#include <lsp-plug.in/fmt/config/Serializer.h>
#include <stdarg.h>
#include <math.h>

namespace lsp
{
    using namespace lsp::config;
}

UTEST_BEGIN("runtime.fmt.config", serializer)

    void test_serialize(LSPString & out)
    {
        Serializer s;

        static const char *output =
                "# Single-line comment\n"
                "\n"
                "# Multi\n"
                "# line\n"
                "# comment\n"
                "\n"
                "key1 = -123\n"
                "key2 = u32:123\n"
                "key3 = i64:\"16\"\n"
                "key4 = \"440\"\n"
                "key5 = 3.14\n"
                "key6 = f32:3.14159\n"
                "key7 = -inf db\n"
                "key8 = \"0.00000\"\n"
                "key9 = текст\n"
                "key10 = \\ \\ \\ text\\ with\\ \\#\\ sign\\ and\\ \\\"quotes\\\"\\ \\ \\ \n"
                "key11 = str:\\ \\ \\ text\\ with\\ \\#\\ sign\\ and\\ \\\"quotes\\\"\\ \\ \\ \n"
                "key12 = str:\"   text with # sign and \\\"quotes\\\"   \"\n"
                "/the/valid/key = value\n"
                "key13 = blob:\"text/plain:10:1234567890\"\n"
                "key14 = blob:\":5:abcde\"\n";

        UTEST_ASSERT(s.wrap(&out) == STATUS_OK);

        UTEST_ASSERT(s.write_comment("Single-line comment") == STATUS_OK);
        UTEST_ASSERT(s.writeln() == STATUS_OK);
        UTEST_ASSERT(s.write_comment("Multi\nline\ncomment") == STATUS_OK);
        UTEST_ASSERT(s.writeln() == STATUS_OK);

        value_t v;

        v.i32       = -123;
        UTEST_ASSERT(s.write("key1", &v, SF_TYPE_I32) == STATUS_OK);
        v.u32       = 123;
        UTEST_ASSERT(s.write("key2", &v, SF_TYPE_U32 | SF_TYPE_SET) == STATUS_OK);
        v.i64       = 16;
        UTEST_ASSERT(s.write("key3", &v, SF_TYPE_I64 | SF_TYPE_SET | SF_QUOTED) == STATUS_OK);
        v.u64       = 440;
        UTEST_ASSERT(s.write("key4", &v, SF_TYPE_U64 | SF_QUOTED) == STATUS_OK);
        v.f32       = 3.141593;
        UTEST_ASSERT(s.write("key5", &v, SF_TYPE_F32 | SF_PREC_SHORT) == STATUS_OK);
        UTEST_ASSERT(s.write("key6", &v, SF_TYPE_F32 | SF_TYPE_SET | SF_PREC_NORMAL) == STATUS_OK);
        v.f64       = 0.0;
        UTEST_ASSERT(s.write("key7", &v, SF_TYPE_F64 | SF_PREC_SHORT | SF_DECIBELS) == STATUS_OK);
        UTEST_ASSERT(s.write("key8", &v, SF_TYPE_F64 | SF_PREC_NORMAL | SF_QUOTED) == STATUS_OK);

        v.str       = const_cast<char *>("текст");
        UTEST_ASSERT(s.write("key9", &v, SF_TYPE_STR) == STATUS_OK);
        v.str       = const_cast<char *>("   text with # sign and \"quotes\"   ");
        UTEST_ASSERT(s.write("key10", &v, SF_TYPE_STR) == STATUS_OK);
        UTEST_ASSERT(s.write("key11", &v, SF_TYPE_STR | SF_TYPE_SET) == STATUS_OK);
        UTEST_ASSERT(s.write("key12", &v, SF_TYPE_STR | SF_TYPE_SET | SF_QUOTED) == STATUS_OK);

        v.str       = const_cast<char *>("value");
        UTEST_ASSERT(s.write("wrong key", &v, SF_TYPE_STR) == STATUS_INVALID_VALUE);
        UTEST_ASSERT(s.write("wrong/key", &v, SF_TYPE_STR) == STATUS_INVALID_VALUE);

        UTEST_ASSERT(s.write("/the/valid/key", &v, SF_TYPE_STR) == STATUS_OK);

        v.blob.length   = 10;
        v.blob.ctype    = const_cast<char *>("text/plain");
        v.blob.data     = const_cast<char *>("1234567890");
        UTEST_ASSERT(s.write("key13", &v, SF_TYPE_BLOB) == STATUS_OK);

        v.blob.length   = 5;
        v.blob.ctype    = NULL;
        v.blob.data     = const_cast<char *>("abcde");
        UTEST_ASSERT(s.write("key14", &v, SF_TYPE_BLOB) == STATUS_OK);

        UTEST_ASSERT(s.close() == STATUS_OK);

        UTEST_ASSERT(out.equals_utf8(output),
                printf("Got output:\n%s\n", out.get_utf8());
                printf("Expected output:\n%s\n", output);
            );
    }

    void test_deserialize(LSPString &out)
    {
        PullParser p;

        UTEST_ASSERT(p.wrap(&out) == STATUS_OK);
        status_t res;
        while ((res = p.next()) != STATUS_EOF)
            printf("  got parameter: %s\n", p.current()->name.get_native());

        UTEST_ASSERT(res == STATUS_EOF);
        UTEST_ASSERT(p.close() == STATUS_OK);
    }


    UTEST_MAIN
    {
        LSPString out;

        printf("Testing serialization...\n");
        test_serialize(out);

        printf("Testing deserialization...\n");
        test_deserialize(out);
    }

UTEST_END;


