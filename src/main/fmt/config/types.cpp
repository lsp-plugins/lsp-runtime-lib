/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-runtime-lib
 * Created on: 29 апр. 2020 г.
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

#include <lsp-plug.in/fmt/config/types.h>
#include <lsp-plug.in/stdlib/string.h>
#include <stdlib.h>

namespace lsp
{
    namespace config
    {
        param_t::param_t()
        {
            flags       = SF_TYPE_NONE;
            ::bzero(&v, sizeof(v));
        }

        param_t::~param_t()
        {
            clear();
        }

        bool param_t::copy(const param_t *src)
        {
            param_t tmp;
            tmp.flags   = SF_TYPE_NONE;
            if (!tmp.name.set(&src->name))
                return false;
            if (!tmp.comment.set(&src->comment))
                return false;
            tmp.flags   = src->flags;

            switch (src->flags & SF_TYPE_MASK)
            {
                case SF_TYPE_NONE: break;
                case SF_TYPE_I32:
                case SF_TYPE_U32:
                case SF_TYPE_I64:
                case SF_TYPE_U64:
                case SF_TYPE_F32:
                case SF_TYPE_F64:
                    tmp.v           = src->v;
                    break;
                case SF_TYPE_STR:
                    tmp.v.str       = NULL;
                    if (src->v.str != NULL)
                    {
                        if ((tmp.v.str = ::strdup(src->v.str)) == NULL)
                            return false;
                    }
                    break;
                case SF_TYPE_BLOB:
                    tmp.v.blob.length   = src->v.blob.length;
                    tmp.v.blob.ctype    = NULL;
                    tmp.v.blob.data     = NULL;

                    if (src->v.blob.ctype != NULL)
                    {
                        if ((tmp.v.blob.ctype = ::strdup(src->v.blob.ctype)) == NULL)
                            return false;
                    }
                    if (src->v.blob.data != NULL)
                    {
                        if ((tmp.v.blob.data = ::strdup(src->v.blob.data)) == NULL)
                            return false;
                    }
                    break;
                default:
                    return false;
            }

            // Swap data with OK object
            swap(&tmp);

            return true;
        }

        void param_t::swap(param_t *dst)
        {
            name.swap(&dst->name);
            comment.swap(&dst->comment);
            lsp::swap(flags, dst->flags);
            lsp::swap(v, dst->v);
        }

        void param_t::clear()
        {
            name.truncate();
            comment.truncate();

            switch (flags & SF_TYPE_MASK)
            {
                case SF_TYPE_STR:
                    if (v.str != NULL)
                        ::free(v.str);
                    v.str           = NULL;
                    break;
                case SF_TYPE_BLOB:
                    if (v.blob.ctype != NULL)
                        ::free(v.blob.ctype);
                    if (v.blob.data != NULL)
                        ::free(v.blob.data);
                    v.blob.length   = 0;
                    v.blob.ctype    = NULL;
                    v.blob.data     = NULL;
                    break;
            }

            flags = SF_TYPE_NONE;
        }
    }
}


