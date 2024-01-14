/*
 * Copyright (C) 2024 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2024 Vladimir Sadovnikov <sadko4u@gmail.com>
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

#include <lsp-plug.in/expr/Resolver.h>

namespace lsp
{
    namespace expr
    {
        
        Resolver::Resolver()
        {
        }
        
        Resolver::~Resolver()
        {
        }
    
        status_t Resolver::resolve(value_t *value, const char *name, size_t num_indexes, const ssize_t *indexes)
        {
            set_value_null(value);
            return STATUS_OK;
        }

        status_t Resolver::resolve(value_t *value, const LSPString *name, size_t num_indexes, const ssize_t *indexes)
        {
            return resolve(value, name->get_utf8(), num_indexes, indexes);
        }

        status_t Resolver::call(value_t *value, const char *name, size_t num_args, const value_t *args)
        {
            set_value_undef(value);
            return STATUS_OK;
        }

        status_t Resolver::call(value_t *value, const LSPString *name, size_t num_args, const value_t *args)
        {
            return call(value, name->get_utf8(), num_args, args);
        }

    } /* namespace expr */
} /* namespace lsp */
