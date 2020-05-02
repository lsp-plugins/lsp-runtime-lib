/*
 * pullparser.cpp
 *
 *  Created on: 30 апр. 2020 г.
 *      Author: sadko
 */


#include <lsp-plug.in/test-fw/utest.h>
#include <lsp-plug.in/test-fw/helpers.h>
#include <lsp-plug.in/fmt/config/PullParser.h>
#include <stdarg.h>
#include <math.h>

namespace lsp
{
    using namespace lsp::config;
}

UTEST_BEGIN("runtime.fmt.config", pullparser)

    void test_valid_cases()
    {
        static const char *text =
            "# comment\n"
            "\n\r"
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

        PullParser p;
        param_t xp;
        const param_t *pp;

        // Wrap the string
        UTEST_ASSERT((pp = p.current()) == NULL);
        UTEST_ASSERT(p.wrap(text, "UTF-8") == STATUS_OK);
        UTEST_ASSERT((pp = p.current()) == NULL);

        // key1
        UTEST_ASSERT(p.next() == STATUS_OK);
        UTEST_ASSERT((pp = p.current()) != NULL);
        UTEST_ASSERT(pp->name.equals_ascii("key1"));
        UTEST_ASSERT(pp->flags == SF_TYPE_STR);
        UTEST_ASSERT(::strcmp(pp->v.str, "value1") == 0);

        // key2
        UTEST_ASSERT(p.next() == STATUS_OK);
        UTEST_ASSERT((pp = p.current()) != NULL);
        UTEST_ASSERT(pp->name.equals_ascii("key2"));
        UTEST_ASSERT(pp->flags == SF_TYPE_I32);
        UTEST_ASSERT(pp->v.i32 == 123);

        // key3
        UTEST_ASSERT(p.next() == STATUS_OK);
        UTEST_ASSERT((pp = p.current()) != NULL);
        UTEST_ASSERT(pp->name.equals_ascii("key3"));
        UTEST_ASSERT(pp->flags == SF_TYPE_F32);
        UTEST_ASSERT(float_equals_relative(pp->v.f32, 123.456, 0.0001));

        // key4
        UTEST_ASSERT(p.next() == STATUS_OK);
        UTEST_ASSERT((pp = p.current()) != NULL);
        UTEST_ASSERT(pp->name.equals_ascii("key4"));
        UTEST_ASSERT(pp->flags == (SF_TYPE_F32 | SF_DECIBELS));
        UTEST_ASSERT(float_equals_relative(pp->v.f32, 4.21696, 1e-5)); // 12.5 dB

        // key5
        UTEST_ASSERT(p.next() == STATUS_OK);
        UTEST_ASSERT((pp = p.current()) != NULL);
        UTEST_ASSERT(pp->name.equals_ascii("key5"));
        UTEST_ASSERT(pp->flags == (SF_TYPE_I32 | SF_TYPE_SET));
        UTEST_ASSERT(pp->v.i32 == 123);

        // key6
        UTEST_ASSERT(p.next() == STATUS_OK);
        UTEST_ASSERT((pp = p.current()) != NULL);
        UTEST_ASSERT(pp->name.equals_ascii("key6"));
        UTEST_ASSERT(pp->flags == (SF_TYPE_F32 | SF_TYPE_SET | SF_QUOTED | SF_DECIBELS));
        UTEST_ASSERT(float_equals_relative(pp->v.f32, 4.21696, 1e-5)); // 12.5 dB

        // key7
        UTEST_ASSERT(p.next(&xp) == STATUS_OK);
        UTEST_ASSERT(xp.name.equals_ascii("key7"));
        UTEST_ASSERT(xp.flags == SF_TYPE_F32);
        UTEST_ASSERT(::isinf(xp.v.f32) && (xp.v.f32 < 0)); // -inf

        // key8
        UTEST_ASSERT(p.next(&xp) == STATUS_OK);
        UTEST_ASSERT(xp.name.equals_ascii("key8"));
        UTEST_ASSERT(xp.flags == (SF_TYPE_STR | SF_QUOTED));
        UTEST_ASSERT(::strcmp(xp.v.str, "+inf") == 0); // +inf

        // key9
        UTEST_ASSERT(p.next() == STATUS_OK);
        UTEST_ASSERT((pp = p.current()) != NULL);
        UTEST_ASSERT(pp->name.equals_ascii("key9"));
        UTEST_ASSERT(pp->flags == (SF_TYPE_BLOB | SF_TYPE_SET | SF_QUOTED));
        UTEST_ASSERT(pp->v.blob.length == 5);
        UTEST_ASSERT(pp->v.blob.ctype == NULL);
        UTEST_ASSERT(::strcmp(pp->v.blob.data, "12345") == 0);

        // key10
        UTEST_ASSERT(p.next() == STATUS_OK);
        UTEST_ASSERT((pp = p.current()) != NULL);
        UTEST_ASSERT(pp->name.equals_ascii("key10"));
        UTEST_ASSERT(pp->flags == (SF_TYPE_BLOB | SF_TYPE_SET | SF_QUOTED));
        UTEST_ASSERT(pp->v.blob.length == 6);
        UTEST_ASSERT(::strcmp(pp->v.blob.ctype, "text/plain") == 0);
        UTEST_ASSERT(::strcmp(pp->v.blob.data, "123456") == 0);

        // key11
        UTEST_ASSERT(p.next() == STATUS_OK);
        UTEST_ASSERT((pp = p.current()) != NULL);
        UTEST_ASSERT(pp->name.equals_ascii("key11"));
        UTEST_ASSERT(pp->flags == SF_TYPE_STR);
        UTEST_ASSERT(::strcmp(pp->v.str, "string with spaces") == 0);

        // key12
        UTEST_ASSERT(p.next() == STATUS_OK);
        UTEST_ASSERT((pp = p.current()) != NULL);
        UTEST_ASSERT(pp->name.equals_ascii("key12"));
        UTEST_ASSERT(pp->flags == (SF_TYPE_STR | SF_QUOTED));
        UTEST_ASSERT(::strcmp(pp->v.str, "string \"with\" escapes") == 0);

        // key13
        UTEST_ASSERT(p.next() == STATUS_OK);
        UTEST_ASSERT((pp = p.current()) != NULL);
        UTEST_ASSERT(pp->name.equals_ascii("key13"));
        UTEST_ASSERT(pp->flags == SF_TYPE_STR);
        UTEST_ASSERT(::strcmp(pp->v.str, "string \"with other\" escapes") == 0);

        // key14
        UTEST_ASSERT(p.next() == STATUS_OK);
        UTEST_ASSERT((pp = p.current()) != NULL);
        UTEST_ASSERT(pp->name.equals_ascii("key14"));
        UTEST_ASSERT(pp->flags == SF_TYPE_STR);
        UTEST_ASSERT(::strcmp(pp->v.str, "string # not comment") == 0);

        // key15
        UTEST_ASSERT(p.next() == STATUS_OK);
        UTEST_ASSERT((pp = p.current()) != NULL);
        UTEST_ASSERT(pp->name.equals_ascii("key15"));
        UTEST_ASSERT(pp->flags == SF_TYPE_STR);
        UTEST_ASSERT(::strcmp(pp->v.str, "") == 0);

        // /tree/arg1
        UTEST_ASSERT(p.next() == STATUS_OK);
        UTEST_ASSERT((pp = p.current()) != NULL);
        UTEST_ASSERT(pp->name.equals_ascii("/tree/arg1"));
        UTEST_ASSERT(pp->flags == (SF_TYPE_STR | SF_TYPE_SET | SF_QUOTED));
        UTEST_ASSERT(::strcmp(pp->v.str, "value") == 0);

        // eof
        UTEST_ASSERT(p.next() == STATUS_EOF);

        UTEST_ASSERT(p.close() == STATUS_OK);
    }

    void test_invalid_cases()
    {
        static const char *lines[] =
        {
            "abcdef",
            "123=",
            "1abc=def",
            "abc=\"def",
            "abc=f32:",
            "abc=i32:",
            "abc=i32:12 db",
            "abc=f32:12 .34",
            "abc=f32:\"12\" db",
            "abc=\"invalid\" escape\"",
            "tree/node=10.11",
            NULL
        };

        for (const char **xp=lines; *xp != NULL; ++xp)
        {
            PullParser p;
            printf("  testing bad line: %s\n", *xp);
            UTEST_ASSERT(p.wrap(*xp) == STATUS_OK);
            UTEST_ASSERT(p.next() == STATUS_BAD_FORMAT);
            UTEST_ASSERT(p.close() == STATUS_OK);
        }
    }

    void test_file_load()
    {
        io::Path path;
        UTEST_ASSERT(path.fmt("%s/%s", resources(), "config/rbm.cfg"));

        PullParser p;
        status_t res;

        printf("Reading file %s...\n", path.as_native());
        UTEST_ASSERT(p.open(&path) == STATUS_OK);
        while ((res = p.next()) != STATUS_EOF)
            printf("  got parameter: %s\n", p.current()->name.get_native());

        UTEST_ASSERT(res == STATUS_EOF);
        UTEST_ASSERT(p.close() == STATUS_OK);
    }

    UTEST_MAIN
    {
        printf("Testing valid cases...\n");
        test_valid_cases();
        printf("Testing invalid cases...\n");
        test_invalid_cases();
        printf("Testing file load...\n");
        test_file_load();
    }

UTEST_END
