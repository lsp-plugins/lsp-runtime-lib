/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-runtime-lib
 * Created on: 17 сен. 2019 г.
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
#include <lsp-plug.in/expr/token.h>
#include <lsp-plug.in/expr/Tokenizer.h>
#include <lsp-plug.in/io/InStringSequence.h>

namespace lsp
{
    using namespace lsp::expr;
}

UTEST_BEGIN("runtime.expr", tokenizer)

    void ck_token(Tokenizer &t, const char *s, token_t token)
    {
        printf("  checking token: %s\n", s);
        token_t tok = t.get_token(TF_GET);
        UTEST_ASSERT_MSG(tok == token, "Error testing token: %s", s);
        UTEST_ASSERT_MSG(t.text_value()->equals_ascii(s), "Error testing token: %s", s);
    }

    void ck_bareword(Tokenizer &t, const char *s)
    {
        printf("  checking token: %s\n", s);
        token_t tok = t.get_token(TF_GET | TF_XKEYWORDS);
        UTEST_ASSERT_MSG(tok == TT_BAREWORD, "Error testing token: %s", s);
        UTEST_ASSERT_MSG(t.text_value()->equals_ascii(s), "Error testing token: %s", s);
    }

    void ck_color(Tokenizer &t, const char *s)
    {
        printf("  checking token: %s\n", s);
        token_t tok = t.get_token(TF_GET | TF_COLOR);
        UTEST_ASSERT_MSG(tok == TT_COLOR, "Error testing token: %s", s);
        UTEST_ASSERT_MSG(t.text_value()->equals_ascii(s), "Error testing token: %s", s);
    }

    void ck_int(Tokenizer &t, ssize_t value)
    {
        printf("  checking integer: %d\n", int(value));
        token_t tok = t.get_token(TF_GET);
        UTEST_ASSERT_MSG(tok == TT_IVALUE, "Error testing token: not IVALUE");
        UTEST_ASSERT_MSG(t.int_value() == value, "Error testing token: %d != %d", int(t.int_value()), int(value));
    }

    void ck_float(Tokenizer &t, double value)
    {
        printf("  checking float: %f\n", double(value));
        token_t tok = t.get_token(TF_GET);
        UTEST_ASSERT_MSG(tok == TT_FVALUE, "Error testing token: not FVALUE");
        UTEST_ASSERT_MSG(float_equals_relative(t.float_value(), value), "Error testing token: %f != %f", t.float_value(), value);
    }

    void ck_invalid(const char *s, token_t token)
    {
        printf("  checking invalid token: %s\n", s);

        io::InStringSequence sq;
        UTEST_ASSERT(sq.wrap(s, "UTF-8") == STATUS_OK);

        Tokenizer t(&sq);
        token_t tok = t.get_token(TF_GET);
        UTEST_ASSERT(tok == token);
    }

    void test_sign_tokens()
    {
        static const char *tokens =
                "( ) [ ] { } & && | || ! ^ ^^ ~ + - * ** / % < > <= >= != <> = == <=> ? : ; , # @ ( )";

        io::InStringSequence sq;
        UTEST_ASSERT(sq.wrap(tokens, "UTF-8") == STATUS_OK);

        Tokenizer t(&sq);

        ck_token(t, "(", TT_LBRACE);
        ck_token(t, ")", TT_RBRACE);
        ck_token(t, "[", TT_LQBRACE);
        ck_token(t, "]", TT_RQBRACE);
        ck_token(t, "{", TT_LCBRACE);
        ck_token(t, "}", TT_RCBRACE);
        ck_token(t, "&", TT_AND);
        ck_token(t, "&&", TT_AND);
        ck_token(t, "|", TT_OR);
        ck_token(t, "||", TT_OR);
        ck_token(t, "!", TT_NOT);
        ck_token(t, "^", TT_XOR);
        ck_token(t, "^^", TT_XOR);
        ck_token(t, "~", TT_BNOT);
        ck_token(t, "+", TT_ADD);
        ck_token(t, "-", TT_SUB);
        ck_token(t, "*", TT_MUL);
        ck_token(t, "**", TT_POW);
        ck_token(t, "/", TT_DIV);
        ck_token(t, "%", TT_IMOD);
        ck_token(t, "<", TT_LESS);
        ck_token(t, ">", TT_GREATER);
        ck_token(t, "<=", TT_LESS_EQ);
        ck_token(t, ">=", TT_GREATER_EQ);
        ck_token(t, "!=", TT_NOT_EQ);
        ck_token(t, "<>", TT_NOT_EQ);
        ck_token(t, "=", TT_EQ);
        ck_token(t, "==", TT_EQ);
        ck_token(t, "<=>", TT_CMP);
        ck_token(t, "?", TT_QUESTION);
        ck_token(t, ":", TT_COLON);
        ck_token(t, ";", TT_SEMICOLON);
        ck_token(t, ",", TT_COMMA);
        ck_token(t, "#", TT_SHARP);
        ck_token(t, "@", TT_AT);
        ck_token(t, "(", TT_LBRACE);
        ck_token(t, ")", TT_RBRACE);

        UTEST_ASSERT(t.get_token(TF_GET) == TT_EOF);
    }

    void test_text_tokens()
    {
        static const char *tokens =
                "true false null undef pi e "
                "and or not xor "
                "band bor bnot bxor "
                "add sub mul pow div fmod "
                "iadd isub imul idiv imod mod "
                "sc scat sr srep sl slen uc supr lc slwr srev "
                "lt nge gt nle le ngt ge nlt ne eq cmp icmp "
                "ilt inge igt inle ile ingt ige inlt ine ie ieq "
                "int float fp bool str "
                "ex db "
                "bareword "
                "sin cos tan tg asin acos atan arcsin arccos arctg "
                "ln log loge lg logd log10 logb log2 "
                "exp sqrt rad deg abs "
                "true false null " // Parse this as barewords
                "@112233 #123 " // Parse this as colors
            ;

        io::InStringSequence sq;
        UTEST_ASSERT(sq.wrap(tokens, "UTF-8") == STATUS_OK);

        Tokenizer t(&sq);

        ck_token(t, "true", TT_TRUE);
        ck_token(t, "false", TT_FALSE);
        ck_token(t, "null", TT_NULL);
        ck_token(t, "undef", TT_UNDEF);
        ck_token(t, "pi", TT_PI);
        ck_token(t, "e", TT_E);

        ck_token(t, "and", TT_AND);
        ck_token(t, "or", TT_OR);
        ck_token(t, "not", TT_NOT);
        ck_token(t, "xor", TT_XOR);

        ck_token(t, "band", TT_BAND);
        ck_token(t, "bor", TT_BOR);
        ck_token(t, "bnot", TT_BNOT);
        ck_token(t, "bxor", TT_BXOR);

        ck_token(t, "add", TT_ADDSYM);
        ck_token(t, "sub", TT_SUBSYM);
        ck_token(t, "mul", TT_MUL);
        ck_token(t, "pow", TT_POW);
        ck_token(t, "div", TT_DIV);
        ck_token(t, "fmod", TT_FMOD);

        ck_token(t, "iadd", TT_IADD);
        ck_token(t, "isub", TT_ISUB);
        ck_token(t, "imul", TT_IMUL);
        ck_token(t, "idiv", TT_IDIV);
        ck_token(t, "imod", TT_IMOD);
        ck_token(t, "mod", TT_IMOD);

        ck_token(t, "sc", TT_SCAT);
        ck_token(t, "scat", TT_SCAT);
        ck_token(t, "sr", TT_SREP);
        ck_token(t, "srep", TT_SREP);
        ck_token(t, "sl", TT_SLEN);
        ck_token(t, "slen", TT_SLEN);
        ck_token(t, "uc", TT_SUPR);
        ck_token(t, "supr", TT_SUPR);
        ck_token(t, "lc", TT_SLWR);
        ck_token(t, "slwr", TT_SLWR);
        ck_token(t, "srev", TT_SREV);

        ck_token(t, "lt", TT_LESS);
        ck_token(t, "nge", TT_LESS);
        ck_token(t, "gt", TT_GREATER);
        ck_token(t, "nle", TT_GREATER);
        ck_token(t, "le", TT_LESS_EQ);
        ck_token(t, "ngt", TT_LESS_EQ);
        ck_token(t, "ge", TT_GREATER_EQ);
        ck_token(t, "nlt", TT_GREATER_EQ);
        ck_token(t, "ne", TT_NOT_EQ);
        ck_token(t, "eq", TT_EQ);
        ck_token(t, "cmp", TT_CMP);
        ck_token(t, "icmp", TT_ICMP);

        ck_token(t, "ilt", TT_ILESS);
        ck_token(t, "inge", TT_ILESS);
        ck_token(t, "igt", TT_IGREATER);
        ck_token(t, "inle", TT_IGREATER);
        ck_token(t, "ile", TT_ILESS_EQ);
        ck_token(t, "ingt", TT_ILESS_EQ);
        ck_token(t, "ige", TT_IGREATER_EQ);
        ck_token(t, "inlt", TT_IGREATER_EQ);
        ck_token(t, "ine", TT_INOT_EQ);
        ck_token(t, "ie", TT_IEQ);
        ck_token(t, "ieq", TT_IEQ);

        ck_token(t, "int", TT_INT);
        ck_token(t, "float", TT_FLOAT);
        ck_token(t, "fp", TT_FLOAT);
        ck_token(t, "bool", TT_BOOL);
        ck_token(t, "str", TT_STR);

        ck_token(t, "ex", TT_EX);
        ck_token(t, "db", TT_DB);
        ck_token(t, "bareword", TT_BAREWORD);

        ck_token(t, "sin", TT_SIN);
        ck_token(t, "cos", TT_COS);
        ck_token(t, "tan", TT_TAN);
        ck_token(t, "tg", TT_TAN);
        ck_token(t, "asin", TT_ASIN);
        ck_token(t, "acos", TT_ACOS);
        ck_token(t, "atan", TT_ATAN);
        ck_token(t, "arcsin", TT_ASIN);
        ck_token(t, "arccos", TT_ACOS);
        ck_token(t, "arctg", TT_ATAN);

        ck_token(t, "ln", TT_LOGE);
        ck_token(t, "log", TT_LOGE);
        ck_token(t, "loge", TT_LOGE);
        ck_token(t, "lg", TT_LOGD);
        ck_token(t, "logd", TT_LOGD);
        ck_token(t, "log10", TT_LOGD);
        ck_token(t, "logb", TT_LOG2);
        ck_token(t, "log2", TT_LOG2);

        ck_token(t, "exp", TT_EXP);
        ck_token(t, "sqrt", TT_SQRT);
        ck_token(t, "rad", TT_RAD);
        ck_token(t, "deg", TT_DEG);
        ck_token(t, "abs", TT_ABS);

        ck_bareword(t, "true");
        ck_bareword(t, "false");
        ck_bareword(t, "null");

        ck_color(t, "@112233");
        ck_color(t, "#123");

        UTEST_ASSERT(t.get_token(TF_GET) == TT_EOF);
    }

    void test_identifier_tokens()
    {
        static const char *tokens =
                ": :_ :A :abc :aBc :abc123 :i123:xyz"
            ;

        io::InStringSequence sq;
        UTEST_ASSERT(sq.wrap(tokens, "UTF-8") == STATUS_OK);

        Tokenizer t(&sq);

        ck_token(t, ":", TT_COLON);
        ck_token(t, "_", TT_IDENTIFIER);
        ck_token(t, "A", TT_IDENTIFIER);
        ck_token(t, "abc", TT_IDENTIFIER);
        ck_token(t, "aBc", TT_IDENTIFIER);
        ck_token(t, "abc123", TT_IDENTIFIER);
        ck_token(t, "i123", TT_IDENTIFIER);
        ck_token(t, "xyz", TT_IDENTIFIER);

        UTEST_ASSERT(t.get_token(TF_GET) == TT_EOF);
    }

    void test_numeric_tokens()
    {
        static const char *tokens =
                " "
                "+ - 0 0.0 +1 -1 +1.0 -1.0 +1.123 -1.321 "
                "0b0101 0b0101.0 0b0101.11 "
                "0o17 0o7.0 0o7.7 "
                "0d129 0d9.0 0d9.9 "
                "0x1f 0x1f.0 0x1f.1 "
                "1.e 2.0e .3e 4.5e 6.7e1 8.9e+1 1.0e-1 .2e+1 .3e-1 "
                "0b101.0e-10 -0b101.0e+10 "
                "0x5.0p-2 -0x5.0p+2 "
            ;

        io::InStringSequence sq;
        UTEST_ASSERT(sq.wrap(tokens, "UTF-8") == STATUS_OK);

        Tokenizer t(&sq);

        ck_token(t, "+", TT_ADD);
        ck_token(t, "-", TT_SUB);
        ck_int(t, 0);
        ck_float(t, 0.0);
        ck_int(t, +1);
        ck_int(t, -1);
        ck_float(t, +1.0);
        ck_float(t, -1.0);
        ck_float(t, +1.123);
        ck_float(t, -1.321);

        ck_int(t, 5);
        ck_float(t, 5.0);
        ck_float(t, 5.75);

        ck_int(t, 15);
        ck_float(t, 7.0);
        ck_float(t, 7.875);

        ck_int(t, 129);
        ck_float(t, 9.0);
        ck_float(t, 9.9);

        ck_int(t, 0x1f);
        ck_float(t, 31.0);
        ck_float(t, 31.0625);

        ck_float(t, 1.0);
        ck_float(t, 2.0);
        ck_float(t, 0.3);
        ck_float(t, 4.5);
        ck_float(t, 67.0);
        ck_float(t, 89.0);
        ck_float(t, 0.1);
        ck_float(t, 2.0);
        ck_float(t, 0.03);

        ck_float(t, 1.25);
        ck_float(t, -20.0);

        ck_float(t, 0.01953125);
        ck_float(t, -1280.0);

        UTEST_ASSERT(t.get_token(TF_GET) == TT_EOF);
    }

    void test_string_tokens()
    {
        static const char *tokens =
                "'' 1 '\\n' 2 '\\r\\t' 3 "
                "'123 ' '456 ' '789' 4 'end \\protector'5 '\\\':xc\\\''";
            ;

        io::InStringSequence sq;
        UTEST_ASSERT(sq.wrap(tokens, "UTF-8") == STATUS_OK);

        Tokenizer t(&sq);

        ck_token(t, "", TT_STRING);
        ck_int(t, 1);
        ck_token(t, "\n", TT_STRING);
        ck_int(t, 2);
        ck_token(t, "\r\t", TT_STRING);
        ck_int(t, 3);
        ck_token(t, "123 456 789", TT_STRING);
        ck_int(t, 4);
        ck_token(t, "end \\protector", TT_STRING);
        ck_int(t, 5);
        ck_token(t, "':xc'", TT_STRING);

        UTEST_ASSERT(t.get_token(TF_GET) == TT_EOF);
    }

    void test_invalid_tokens()
    {
        ck_invalid(".", TT_UNKNOWN);
        ck_invalid(".e+", TT_UNKNOWN);
        ck_invalid("\'", TT_ERROR);
        ck_invalid("\'\' \'", TT_ERROR);
    }

    void test_expression_tokens()
    {
        static const char *tokens =
            "((:a eq :b) or (:a eq :c+:d[ssel])) * 10 + (:b ine :c) ? 1 db : 2.0 db";

        io::InStringSequence sq;
        UTEST_ASSERT(sq.wrap(tokens, "UTF-8") == STATUS_OK);

        Tokenizer t(&sq);

        ck_token(t, "(", TT_LBRACE);
            ck_token(t, "(", TT_LBRACE);
                ck_token(t, "a", TT_IDENTIFIER);
                ck_token(t, "eq", TT_EQ);
                ck_token(t, "b", TT_IDENTIFIER);
            ck_token(t, ")", TT_RBRACE);

            ck_token(t, "or", TT_OR);

            ck_token(t, "(", TT_LBRACE);
                ck_token(t, "a", TT_IDENTIFIER);
                ck_token(t, "eq", TT_EQ);
                ck_token(t, "c", TT_IDENTIFIER);
                ck_token(t, "+", TT_ADD);
                ck_token(t, "d", TT_IDENTIFIER);
                ck_token(t, "[", TT_LQBRACE);
                    ck_token(t, "ssel", TT_BAREWORD);
                ck_token(t, "]", TT_RQBRACE);
            ck_token(t, ")", TT_RBRACE);
        ck_token(t, ")", TT_RBRACE);

        ck_token(t, "*", TT_MUL);
        ck_int(t, 10);
        ck_token(t, "+", TT_ADD);

        ck_token(t, "(", TT_LBRACE);
            ck_token(t, "b", TT_IDENTIFIER);
            ck_token(t, "ine", TT_INOT_EQ);
            ck_token(t, "c", TT_IDENTIFIER);
        ck_token(t, ")", TT_RBRACE);

        ck_token(t, "?", TT_QUESTION);
        ck_int(t, 1);
        ck_token(t, "db", TT_DB);
        ck_token(t, ":", TT_COLON);
        ck_float(t, 2.0);
        ck_token(t, "db", TT_DB);

        UTEST_ASSERT(t.get_token(TF_GET) == TT_EOF);
    }

    UTEST_MAIN
    {
        printf("Testing basic sign tokens...\n");
        test_sign_tokens();
        printf("Testing basic text tokens...\n");
        test_text_tokens();
        printf("Testing identifier tokens...\n");
        test_identifier_tokens();
        printf("Testing numeric tokens...\n");
        test_numeric_tokens();
        printf("Testing string tokens...\n");
        test_string_tokens();
        printf("Testing invalid tokens...\n");
        test_invalid_tokens();

        printf("Testing expression tokens...\n");
        test_expression_tokens();
    }

UTEST_END


