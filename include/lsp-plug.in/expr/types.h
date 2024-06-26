/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-runtime-lib
 * Created on: 16 сент. 2019 г.
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

#ifndef LSP_PLUG_IN_EXPR_TYPES_H_
#define LSP_PLUG_IN_EXPR_TYPES_H_

#include <lsp-plug.in/runtime/version.h>
#include <lsp-plug.in/common/types.h>
#include <lsp-plug.in/common/status.h>
#include <lsp-plug.in/runtime/LSPString.h>

namespace lsp
{
    namespace expr
    {
        enum value_type_t
        {
            VT_UNDEF,       // Value is undefined
            VT_NULL,        // Value is defined but is NULL

            VT_INT,         // Value is of integer type
            VT_FLOAT,       // Value is of float type
            VT_STRING,      // Value is of string type
            VT_BOOL         // Value is of boolean type
        };

        typedef struct value_t
        {
            value_type_t    type;
            union
            {
                ssize_t         v_int;
                double          v_float;
                bool            v_bool;
                LSPString      *v_str;
            };
        } value_t;

        /**
         * User-defined function
         *
         * @param context additional user context passed to function
         * @param result destination value to store result, should be initialized
         * @param num_args number of arguments passed to function
         * @param args list of arguments passed to function (may be NULL if num_args is 0)
         * @return status of operation
         */
        typedef status_t (*function_t)(void *context, value_t *result, size_t num_args, const value_t *args);

        /**
         * Initialize value with VT_UNDEF type
         * @param dst destination value
         */
        void        init_value(value_t *dst);

        /**
         * Initialize value with another value by copying contents
         * @param dst destination value
         * @param src source value to take data from
         * @return status of operation
         */
        status_t    init_value(value_t *dst, const value_t *src);

        /**
         * Copy value. Frees previously used value if it was set
         * @param dst destination value to perform copy
         * @param src source value to take data from
         * @return status of operation
         */
        status_t    copy_value(value_t *dst, const value_t *src);

        /**
         * Set value to NULL
         * @param dst target to set
         */
        void        set_value_null(value_t *dst);

        /**
         * Set value to UNDEF
         * @param dst target to set
         */
        void        set_value_undef(value_t *dst);

        /**
         * Set value to integer
         * @param dst target to set
         * @param value integer value to set
         */
        void        set_value_int(value_t *dst, ssize_t value);

        /**
         * Set value to floating point
         * @param dst target to set
         * @param value floating-point value to set
         */
        void        set_value_float(value_t *dst, double value);

        /**
         * Set value to boolean
         * @param dst target to set
         * @param value boolean value to set
         */
        void        set_value_bool(value_t *dst, bool value);

        /**
         * Set value to string
         * @param dst target to set
         * @param value string value to set
         */
        status_t    set_value_string(value_t *dst, LSPString *value);

        /**
         * Set value to string
         * @param dst target to set
         * @param value UTF-8 string to set
         */
        status_t    set_value_string(value_t *dst, const char *value);

        /**
         * Destroy value and all internal contents associated with it
         * The value remains valid and available for further operations but set to UNDEF
         * @param value value to destroy
         */
        void        destroy_value(value_t *value);

        status_t    fetch_int(ssize_t *dst, const value_t *v);
        status_t    fetch_float(float *dst, const value_t *v);
        status_t    fetch_bool(bool *dst, const value_t *v);
        status_t    fetch_string(LSPString *dst, const value_t *v);

        status_t    cast_value(value_t *v, value_type_t type);
        status_t    cast_value(value_t *dst, const value_t *v, value_type_t type);

        status_t    cast_int(value_t *v);
        status_t    cast_float(value_t *v);
        status_t    cast_bool(value_t *v);
        status_t    cast_string(value_t *v);
        status_t    cast_string_ext(value_t *v);
        status_t    cast_numeric(value_t *v);
        status_t    cast_non_string(value_t *v);

        status_t    cast_int(value_t *dst, const value_t *v);
        status_t    cast_float(value_t *dst, const value_t *v);
        status_t    cast_bool(value_t *dst, const value_t *v);
        status_t    cast_string(value_t *dst, const value_t *v);
        status_t    cast_string_ext(value_t *dst, const value_t *v);
        status_t    cast_numeric(value_t *dst, const value_t *v);
        status_t    cast_non_string(value_t *dst, const value_t *v);

    } /* namespace expr */
} /* namespace lsp */

#endif /* LSP_PLUG_IN_EXPR_TYPES_H_ */
