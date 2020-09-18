/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-runtime-lib
 * Created on: 18 сент. 2019 г.
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

#ifndef LSP_PLUG_IN_EXPR_EVALUATOR_H_
#define LSP_PLUG_IN_EXPR_EVALUATOR_H_

#include <lsp-plug.in/runtime/version.h>
#include <lsp-plug.in/expr/types.h>
#include <lsp-plug.in/expr/Resolver.h>

namespace lsp
{
    namespace expr
    {
        struct expr_t;

        typedef Resolver       eval_env_t;

        typedef status_t (* evaluator_t)(value_t *value, const expr_t *expr, eval_env_t *env);


        status_t eval_ternary(value_t *value, const expr_t *expr, eval_env_t *env);
        status_t eval_xor(value_t *value, const expr_t *expr, eval_env_t *env);
        status_t eval_or(value_t *value, const expr_t *expr, eval_env_t *env);
        status_t eval_and(value_t *value, const expr_t *expr, eval_env_t *env);
        status_t eval_bit_xor(value_t *value, const expr_t *expr, eval_env_t *env);
        status_t eval_bit_or(value_t *value, const expr_t *expr, eval_env_t *env);
        status_t eval_bit_and(value_t *value, const expr_t *expr, eval_env_t *env);

        status_t eval_cmp(value_t *value, const expr_t *expr, eval_env_t *env);
        status_t eval_cmp_eq(value_t *value, const expr_t *expr, eval_env_t *env);
        status_t eval_cmp_ne(value_t *value, const expr_t *expr, eval_env_t *env);
        status_t eval_cmp_lt(value_t *value, const expr_t *expr, eval_env_t *env);
        status_t eval_cmp_gt(value_t *value, const expr_t *expr, eval_env_t *env);
        status_t eval_cmp_le(value_t *value, const expr_t *expr, eval_env_t *env);
        status_t eval_cmp_ge(value_t *value, const expr_t *expr, eval_env_t *env);

        status_t eval_icmp(value_t *value, const expr_t *expr, eval_env_t *env);
        status_t eval_icmp_eq(value_t *value, const expr_t *expr, eval_env_t *env);
        status_t eval_icmp_ne(value_t *value, const expr_t *expr, eval_env_t *env);
        status_t eval_icmp_lt(value_t *value, const expr_t *expr, eval_env_t *env);
        status_t eval_icmp_gt(value_t *value, const expr_t *expr, eval_env_t *env);
        status_t eval_icmp_le(value_t *value, const expr_t *expr, eval_env_t *env);
        status_t eval_icmp_ge(value_t *value, const expr_t *expr, eval_env_t *env);

        status_t eval_add(value_t *value, const expr_t *expr, eval_env_t *env);
        status_t eval_sub(value_t *value, const expr_t *expr, eval_env_t *env);
        status_t eval_iadd(value_t *value, const expr_t *expr, eval_env_t *env);
        status_t eval_isub(value_t *value, const expr_t *expr, eval_env_t *env);

        status_t eval_mul(value_t *value, const expr_t *expr, eval_env_t *env);
        status_t eval_div(value_t *value, const expr_t *expr, eval_env_t *env);
        status_t eval_imul(value_t *value, const expr_t *expr, eval_env_t *env);
        status_t eval_idiv(value_t *value, const expr_t *expr, eval_env_t *env);
        status_t eval_fmod(value_t *value, const expr_t *expr, eval_env_t *env);
        status_t eval_imod(value_t *value, const expr_t *expr, eval_env_t *env);

        status_t eval_power(value_t *value, const expr_t *expr, eval_env_t *env);

        status_t eval_strcat(value_t *value, const expr_t *expr, eval_env_t *env);
        status_t eval_strrep(value_t *value, const expr_t *expr, eval_env_t *env);
        status_t eval_strupper(value_t *value, const expr_t *expr, eval_env_t *env);
        status_t eval_strlower(value_t *value, const expr_t *expr, eval_env_t *env);
        status_t eval_strlen(value_t *value, const expr_t *expr, eval_env_t *env);
        status_t eval_strrev(value_t *value, const expr_t *expr, eval_env_t *env);

        status_t eval_int_cast(value_t *value, const expr_t *expr, eval_env_t *env);
        status_t eval_float_cast(value_t *value, const expr_t *expr, eval_env_t *env);
        status_t eval_string_cast(value_t *value, const expr_t *expr, eval_env_t *env);
        status_t eval_bool_cast(value_t *value, const expr_t *expr, eval_env_t *env);

        status_t eval_not(value_t *value, const expr_t *expr, eval_env_t *env);
        status_t eval_neg(value_t *value, const expr_t *expr, eval_env_t *env);
        status_t eval_psign(value_t *value, const expr_t *expr, eval_env_t *env);
        status_t eval_nsign(value_t *value, const expr_t *expr, eval_env_t *env);

        status_t eval_exists(value_t *value, const expr_t *expr, eval_env_t *env);
        status_t eval_db(value_t *value, const expr_t *expr, eval_env_t *env);

        status_t eval_resolve(value_t *value, const expr_t *expr, eval_env_t *env);
        status_t eval_value(value_t *value, const expr_t *expr, eval_env_t *env);
    }
}

#endif /* LSP_PLUG_IN_EXPR_EVALUATOR_H_ */
