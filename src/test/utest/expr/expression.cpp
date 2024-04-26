/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-runtime-lib
 * Created on: 19 сент. 2019 г.
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

#include <lsp-plug.in/expr/Expression.h>
#include <lsp-plug.in/expr/Variables.h>
#include <lsp-plug.in/stdlib/math.h>
#include <lsp-plug.in/test-fw/helpers.h>
#include <lsp-plug.in/test-fw/utest.h>

#define GAIN_AMP_P_12_DB                    3.98107             /* +12 dB       */
#define GAIN_AMP_M_12_DB                    0.25119             /* -12 dB       */

namespace lsp
{
    using namespace lsp::expr;
}

UTEST_BEGIN("runtime.expr", expression)

    typedef struct context_t
    {
        test_type_t    *pThis;
        Variables      *pVars;
    } context_t;


    void test_float(const char *expr, Resolver *r, double value, float tol = 0.001)
    {
        Expression e(r);
        value_t res;
        init_value(&res);

        printf("Evaluating expression: %s -> %f\n", expr, value);
        UTEST_ASSERT_MSG(e.parse(expr, NULL, Expression::FLAG_NONE) == STATUS_OK, "Error parsing expression: %s", expr);
        UTEST_ASSERT(e.evaluate(&res) == STATUS_OK);
        UTEST_ASSERT(res.type == VT_FLOAT);
        UTEST_ASSERT_MSG(float_equals_relative(res.v_float, value, tol),
                "%s: result (%f) != expected (%f)", expr, double(res.v_float), value);
        destroy_value(&res);
    }

    void test_int(const char *expr, Resolver *r, ssize_t value)
    {
        Expression e(r);
        value_t res;
        init_value(&res);

        printf("Evaluating expression: %s -> %ld\n", expr, long(value));
        UTEST_ASSERT_MSG(e.parse(expr, NULL, Expression::FLAG_NONE) == STATUS_OK, "Error parsing expression: %s", expr);
        UTEST_ASSERT(e.evaluate(&res) == STATUS_OK);
        UTEST_ASSERT(res.type == VT_INT);
        UTEST_ASSERT_MSG(res.v_int == value,
                "%s: result (%ld) != expected (%ld)", expr, long(res.v_int), long(value));
        destroy_value(&res);
    }

    void test_bool(const char *expr, Resolver *r, bool value)
    {
        Expression e(r);
        value_t res;
        init_value(&res);

        printf("Evaluating expression: %s -> %s\n", expr, (value) ? "true" : "false");
        UTEST_ASSERT_MSG(e.parse(expr, NULL, Expression::FLAG_NONE) == STATUS_OK, "Error parsing expression: %s", expr);
        UTEST_ASSERT(e.evaluate(&res) == STATUS_OK);
        UTEST_ASSERT(res.type == VT_BOOL);
        UTEST_ASSERT_MSG(res.v_bool == value,
                "%s: result (%s) != expected (%s)", expr, (res.v_bool) ? "true" : "false", (value) ? "true" : "false");
        destroy_value(&res);
    }

    void test_string(const char *expr, Resolver *r, const char *value)
    {
        LSPString tmp;
        Expression e(r);
        value_t res;
        init_value(&res);

        printf("Evaluating expression: %s -> '%s'\n", expr, value);
        UTEST_ASSERT(tmp.set_utf8(expr) == true);
        UTEST_ASSERT_MSG(e.parse(&tmp, Expression::FLAG_NONE) == STATUS_OK, "Error parsing expression: %s", expr);
        UTEST_ASSERT(e.evaluate(&res) == STATUS_OK);
        UTEST_ASSERT(res.type == VT_STRING);
        UTEST_ASSERT(tmp.set_utf8(value) == true);
        UTEST_ASSERT_MSG(tmp.equals(res.v_str),
                "%s: result ('%s') != expected ('%s')", expr, res.v_str->get_utf8(), tmp.get_utf8());
        destroy_value(&res);
    }

    void test_substitution(const char *expr, Resolver *r, const char *value)
    {
        LSPString tmp;
        Expression e(r);
        value_t res;
        init_value(&res);

        printf("Evaluating expression: %s -> '%s'\n", expr, value);
        UTEST_ASSERT(tmp.set_utf8(expr) == true);
        UTEST_ASSERT_MSG(e.parse(&tmp, Expression::FLAG_STRING) == STATUS_OK, "Error parsing expression: %s", expr);
        UTEST_ASSERT(e.evaluate(&res) == STATUS_OK);
        UTEST_ASSERT(cast_string(&res) == STATUS_OK);
        UTEST_ASSERT(res.type == VT_STRING);
        UTEST_ASSERT(tmp.set_utf8(value) == true);
        UTEST_ASSERT_MSG(tmp.equals(res.v_str),
                "%s: result ('%s') != expected ('%s')", expr, res.v_str->get_utf8(), tmp.get_utf8());
        destroy_value(&res);
    }

    void test_dependencies(Resolver *r)
    {
        LSPString tmp;
        Expression e(r);

        static const char *expr = "(:v[:fa][:ia-:fd]) && (:v[1][:bc] = 'test') || (:za + :zb == undef)";

        printf("Testing dependencies for expression\n");
        UTEST_ASSERT(tmp.set_utf8(expr) == true);
        UTEST_ASSERT_MSG(e.parse(&tmp, Expression::FLAG_NONE) == STATUS_OK, "Error parsing expression: %s", expr);
        UTEST_ASSERT(e.has_dependency("v"));
        UTEST_ASSERT(e.has_dependency("fa"));
        UTEST_ASSERT(e.has_dependency("ia"));
        UTEST_ASSERT(e.has_dependency("fd"));
        UTEST_ASSERT(e.has_dependency("bc"));
        UTEST_ASSERT(e.has_dependency("za"));
        UTEST_ASSERT(e.has_dependency("zb"));
        UTEST_ASSERT(!e.has_dependency("zc"));
    }

    void init_vars(Variables &v)
    {
        UTEST_ASSERT(v.set_int("ia", 1) == STATUS_OK);
        UTEST_ASSERT(v.set_int("ib", 3) == STATUS_OK);
        UTEST_ASSERT(v.set_int("ic", 5) == STATUS_OK);
        UTEST_ASSERT(v.set_int("id", 7) == STATUS_OK);
        UTEST_ASSERT(v.set_int("ie", 10) == STATUS_OK);

        UTEST_ASSERT(v.set_bool("ba", true) == STATUS_OK);
        UTEST_ASSERT(v.set_bool("bb", false) == STATUS_OK);
        UTEST_ASSERT(v.set_bool("bc", true) == STATUS_OK);
        UTEST_ASSERT(v.set_bool("bd", false) == STATUS_OK);

        UTEST_ASSERT(v.set_float("fa", 1) == STATUS_OK);
        UTEST_ASSERT(v.set_float("fb", 0.3) == STATUS_OK);
        UTEST_ASSERT(v.set_float("fc", 0.5) == STATUS_OK);
        UTEST_ASSERT(v.set_float("fd", 0.7) == STATUS_OK);
        UTEST_ASSERT(v.set_float("fe", 0.01) == STATUS_OK);
        UTEST_ASSERT(v.set_float("fg", 14.1) == STATUS_OK);
        UTEST_ASSERT(v.set_float("zoom1", GAIN_AMP_M_12_DB) == STATUS_OK);
        UTEST_ASSERT(v.set_float("zoom2", GAIN_AMP_P_12_DB) == STATUS_OK);

        UTEST_ASSERT(v.set_null("za") == STATUS_OK);
        UTEST_ASSERT(v.set_null("zb") == STATUS_OK);

        UTEST_ASSERT(v.set_int("v_0_0", 1234) == STATUS_OK);
        UTEST_ASSERT(v.set_float("v_0_1", 1.234) == STATUS_OK);
        UTEST_ASSERT(v.set_bool("v_1_0", true) == STATUS_OK);
        UTEST_ASSERT(v.set_string("v_1_1", "test") == STATUS_OK);

        LSPString lower, upper;
        UTEST_ASSERT(lower.set_ascii("lower"));
        UTEST_ASSERT(upper.set_ascii("UPPER"));

        UTEST_ASSERT(v.set_string("sa", &lower) == STATUS_OK);
        UTEST_ASSERT(v.set_string("sb", &upper) == STATUS_OK);

    }

    void test_invalid(const char *expr)
    {
        printf("Testing invalid expression: %s\n", expr);

        Expression e;
        UTEST_ASSERT(e.parse(expr, Expression::FLAG_NONE) != STATUS_OK);
    }

    static status_t func_hello(void *context, value_t *result, size_t num_args, const value_t *args)
    {
        context_t *ctx = static_cast<context_t *>(context);
        const char *__test_group = ctx->pThis->group();
        const char *__test_name = ctx->pThis->name();

        UTEST_ASSERT(context != NULL);
        UTEST_ASSERT(result != NULL);
        UTEST_ASSERT(num_args == 4);
        UTEST_ASSERT(args != NULL);

        // Check arguments
        UTEST_ASSERT(args[0].type == VT_FLOAT);
        UTEST_ASSERT_MSG(float_equals_relative(args[0].v_float, 3.0f),
            "Argument 0: result (%f) != expected (%f)", double(args[0].v_float), 3.0f);
        UTEST_ASSERT(args[1].type == VT_FLOAT);
        UTEST_ASSERT_MSG(float_equals_relative(args[1].v_float, 2.0f),
            "Argument 1: result (%f) != expected (%f)", double(args[1].v_float), 2.0f);
        UTEST_ASSERT(args[2].type == VT_UNDEF);
        UTEST_ASSERT(args[3].type == VT_INT);
        UTEST_ASSERT_MSG(args[3].v_int == -1,
            "Argument 3: result (%d) != expected (%d)", int(args[3].v_int), int(-1));

        UTEST_ASSERT(set_value_string(result, "Hello, ") == STATUS_OK);

        return STATUS_OK;
    }

    static status_t func_world(void *context, value_t *result, size_t num_args, const value_t *args)
    {
        context_t *ctx = static_cast<context_t *>(context);
        const char *__test_group = ctx->pThis->group();
        const char *__test_name = ctx->pThis->name();

        UTEST_ASSERT(context != NULL);
        UTEST_ASSERT(result != NULL);
        UTEST_ASSERT(num_args == 5);
        UTEST_ASSERT(args != NULL);

        // Check arguments
        UTEST_ASSERT(args[0].type == VT_NULL);
        UTEST_ASSERT(args[1].type == VT_FLOAT);
        UTEST_ASSERT_MSG(float_equals_relative(args[1].v_float, 0.7f),
            "Argument 1: result (%f) != expected (%f)", double(args[1].v_float), 0.7f);
        UTEST_ASSERT(args[2].type == VT_BOOL);
        UTEST_ASSERT(args[2].v_bool == true);
        UTEST_ASSERT(args[3].type == VT_UNDEF);
        UTEST_ASSERT(args[4].type == VT_STRING);
        UTEST_ASSERT(args[4].v_str->equals_ascii("test"));

        UTEST_ASSERT(set_value_string(result, "World!") == STATUS_OK);

        return STATUS_OK;
    }


    void test_function_call(Resolver *r)
    {
        context_t ctx;
        Variables v(r);
        Expression e(&v);
        LSPString tmp;

        ctx.pThis       = this;
        ctx.pVars       = &v;

        // Inject additional functions
        UTEST_ASSERT(v.bind_func("hello", func_hello, &ctx) == STATUS_OK);
        UTEST_ASSERT(v.bind_func("world", func_world, &ctx) == STATUS_OK);

        static const char *expr = "hello(log2(:ia + :id), :fa + :ia, :za - :zb, -1) scat world(null, :fd, :bc, undef, 'test')";
        static const char *expected = "Hello, World!";

        printf("Testing dependencies for expression\n");
        UTEST_ASSERT(tmp.set_utf8(expr) == true);
        UTEST_ASSERT_MSG(e.parse(&tmp, Expression::FLAG_NONE) == STATUS_OK, "Error parsing expression: %s", expr);
        UTEST_ASSERT(e.has_dependency("ia"));
        UTEST_ASSERT(e.has_dependency("id"));
        UTEST_ASSERT(e.has_dependency("fa"));
        UTEST_ASSERT(e.has_dependency("za"));
        UTEST_ASSERT(e.has_dependency("zb"));
        UTEST_ASSERT(e.has_dependency("fd"));
        UTEST_ASSERT(e.has_dependency("bc"));
        UTEST_ASSERT(!e.has_dependency("zc"));

        // Evaluate expression
        value_t res;
        init_value(&res);

        printf("Evaluating expression: %s -> '%s'\n", expr, expected);
        UTEST_ASSERT(tmp.set_utf8(expr) == true);
        UTEST_ASSERT_MSG(e.parse(&tmp, Expression::FLAG_NONE) == STATUS_OK, "Error parsing expression: %s", expr);
        UTEST_ASSERT(e.evaluate(&res) == STATUS_OK);
        UTEST_ASSERT(res.type == VT_STRING);
        UTEST_ASSERT_MSG(res.v_str->equals_utf8(expected),
                "%s: result ('%s') != expected ('%s')", expr, res.v_str->get_utf8(), expected);
        destroy_value(&res);
    }

    void test_standard_functions(Resolver *r)
    {
        Expression e(r);
        value_t res;
        init_value(&res);
        lsp_finally { destroy_value(&res); };

        // Evaluate min() expression
        static const char *min_expr = "min(5.0, 4, 10.0, 3, 0.1, 2, 1)";
        printf("Evaluating min() expression: %s\n", min_expr);
        UTEST_ASSERT_MSG(e.parse(min_expr, Expression::FLAG_NONE) == STATUS_OK, "Error parsing expression: %s", min_expr);
        UTEST_ASSERT(e.evaluate(&res) == STATUS_OK);
        UTEST_ASSERT(res.type == VT_FLOAT);
        UTEST_ASSERT(float_equals_adaptive(res.v_float, 0.1));

        // Evaluate min() expression
        static const char *max_expr = "max(5.0, 4, 10.0, 3, 0.1, 2, 1)";
        printf("Evaluating max() expression: %s\n", max_expr);
        UTEST_ASSERT_MSG(e.parse(max_expr, Expression::FLAG_NONE) == STATUS_OK, "Error parsing expression: %s", max_expr);
        UTEST_ASSERT(e.evaluate(&res) == STATUS_OK);
        UTEST_ASSERT(res.type == VT_FLOAT);
        UTEST_ASSERT(float_equals_adaptive(res.v_float, 10.0));

        // Evaluate avg() expression
        static const char *avg_expr = "avg(1, 2, 3, 4, 5, 6, 7)";
        printf("Evaluating avg() expression: %s\n", avg_expr);
        UTEST_ASSERT_MSG(e.parse(avg_expr, Expression::FLAG_NONE) == STATUS_OK, "Error parsing expression: %s", avg_expr);
        UTEST_ASSERT(e.evaluate(&res) == STATUS_OK);
        UTEST_ASSERT(res.type == VT_FLOAT);
        UTEST_ASSERT(float_equals_adaptive(res.v_float, 4.0));

        // Evaluate rms() expression
        static const char *rms_expr = "rms(1, 2, 3, 4, 5, 6, 7)";
        printf("Evaluating avg() expression: %s\n", rms_expr);
        UTEST_ASSERT_MSG(e.parse(rms_expr, Expression::FLAG_NONE) == STATUS_OK, "Error parsing expression: %s", rms_expr);
        UTEST_ASSERT(e.evaluate(&res) == STATUS_OK);
        UTEST_ASSERT(res.type == VT_FLOAT);
        UTEST_ASSERT(float_equals_adaptive(res.v_float, sqrtf(20.0)));
    }

    UTEST_MAIN
    {
        Variables v;
        init_vars(v);

        test_float("12 db", &v, GAIN_AMP_P_12_DB);
        test_float("-12 db", &v, GAIN_AMP_M_12_DB);
        test_float("db 12", &v, GAIN_AMP_P_12_DB);
        test_float("db -12", &v, GAIN_AMP_M_12_DB);
        test_float(":fa + :fb/:fc - :fe", &v, 1.59);
        test_float(":ic ** :ib", &v, 125.0f);
        test_float("fp (:ie + :id)", &v, 17.0);

        test_float("sqrt 4 * 25", &v, 50.0);
        test_float("sqrt (4 * 25)", &v, 10.0);
        test_float("lg 100", &v, 2.0);
        test_float("log2 256", &v, 8.0);
        test_float("ln exp 11", &v, 11.0);
        test_float("deg pi", &v, 180.0f);
        test_float("rad deg pi", &v, M_PI);
        test_float("sin(pi / 6)", &v, 0.5);
        test_float("cos(pi / 3)", &v, 0.5);
        test_float("tg(pi / 4)", &v, 1.0);
        test_float("arctg 1", &v, M_PI / 4);
        test_float("arcsin 0.5", &v, M_PI / 6);
        test_float("arccos 0.5", &v, M_PI / 3);
        test_float("abs -10.1 - abs 4", &v, 6.1);

        test_int("0b1011_0010", &v, 0xb2);
        test_int("0o1_1", &v, 9);
        test_int("0d12_34", &v, 1234);
        test_int("0x12_34", &v, 0x1234);
        test_int("+6 + -3 - --2", &v, 1);
        test_int("(:ia+:ic) idiv :ib", &v, 2);
        test_int(":ia * :ic + :id idiv :ib", &v, 7);
        test_int(":ie bxor 0x3", &v, 9);
        test_int(":ie bor :ic", &v, 15);
        test_int(":ie band 0xC", &v, 8);
        test_int("~:ia band 0xf", &v, 0x0e);
        test_int(":ia icmp null", &v, 1);
        test_int("null cmp :ia", &v, -1);
        test_int("null <=> undef", &v, 1);
        test_int("undef <=> :za", &v, -1);
        test_int("undef <=> :za", &v, -1);
        test_int("int :ba + int :fg", &v, 15);
        test_int("abs -10 - abs 4", &v, 6);

        test_bool(":zoom1 le -9 db", &v, true);
        test_bool(":zoom2 le -9 db", &v, false);
        test_bool(":ia*:ib < :fc / :fe", &v, true);
        test_bool(":ia*:ic > :fa / :fb", &v, true);
        test_bool(":bb || :ba && :bd ^^ :bc", &v, true);
        test_bool("(:bb || :bd) || !(:ba eq :bc)", &v, false);
        test_bool("-:ia + :ib - :ic ieq -(:ie - :id)", &v, true);
        test_bool("'true' eq true", &v, true);
        test_bool("'TRUE' ne true", &v, true);
        test_bool("'TRUE' ieq true", &v, true);
        test_bool("'0x100' ieq 0x100", &v, true);
        test_bool("'0x100' != 0x100", &v, true);
        test_bool("ex :ia", &v, true);
        test_bool("ex :fz", &v, false);
        test_bool(":za ieq :zb", &v, true);
        test_bool(":ia >= :za", &v, true);
        test_bool(":ia + :za == undef", &v, true);
        test_bool("(:v[0][0] ieq 1234) and (:v[bb][ia] = 1.234)", &v, true);
        test_bool("(:v[:fa][:ia-:fd]) && (:v[1][:bc] = 'test')", &v, true);
        test_bool("bool :fb", &v, false);

        for (size_t i=0, j=0; i<40; i += 10, ++j)
        {
            UTEST_ASSERT(v.set_int("x", i) == STATUS_OK);
            test_int(":x < 20 ? :x < 10 ? 0 : 1 : :x < 30 ? 2 : 3", &v, j);
        }

        test_int("slen 'abcdef'", &v, 6);
        test_string("'ABC'", &v, "ABC");
        test_string("'1' sc 20+:ib sc :ic*9", &v, "12345");
        test_string("'xy' sr :id", &v, "xyxyxyxyxyxyxy");
        test_string("lc :sa sc uc :sb", &v, "lowerUPPER");
        test_string("uc :sa sc lc :sb", &v, "LOWERupper");
        test_string("srev :sa sc srev :sb", &v, "rewolREPPU");
        test_string("'null: ' sc :za sc ', undef: ' sc :zx", &v, "null: null, undef: undef");
        test_string("str :bc", &v, "true");

        test_substitution("some bare string", &v, "some bare string");
        test_substitution("${ia}", &v, "1");
        test_substitution("Value is: ${ia}", &v, "Value is: 1");
        test_substitution("Value is: ${:ba}", &v, "Value is: true");
        test_substitution("$${ia}", &v, "${ia}");
        test_substitution("${ia}+${:ie}-${:ic}=${:ia+:ie-:ic}", &v, "1+10-5=6");

        test_dependencies(&v);
        test_function_call(&v);
        test_standard_functions(&v);

        test_invalid("(:a ge 0 db) : -1 : 1");
    }

UTEST_END;


