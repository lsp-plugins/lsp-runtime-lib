/*
 * types.cpp
 *
 *  Created on: 29 апр. 2020 г.
 *      Author: sadko
 */

#include <lsp-plug.in/fmt/config/types.h>
#include <stdlib.h>

namespace lsp
{
    namespace config
    {
        param_t::param_t()
        {
            flags       = SF_TYPE_NONE;
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
                case SF_TYPE_I32: tmp.i32   = src->i32; break;
                case SF_TYPE_U32: tmp.u32   = src->u32; break;
                case SF_TYPE_I64: tmp.i64   = src->i64; break;
                case SF_TYPE_U64: tmp.u64   = src->u64; break;
                case SF_TYPE_F32: tmp.f32   = src->f32; break;
                case SF_TYPE_F64: tmp.f64   = src->f64; break;
                case SF_TYPE_STR:
                    tmp.str         = NULL;
                    if (src->str != NULL)
                    {
                        if ((tmp.str = ::strdup(src->str)) == NULL)
                            return false;
                    }
                    break;
                case SF_TYPE_BLOB:
                    tmp.blob.length = src->blob.length;
                    tmp.blob.ctype  = NULL;
                    tmp.blob.data   = NULL;

                    if (src->blob.ctype != NULL)
                    {
                        if ((tmp.blob.ctype = ::strdup(src->blob.ctype)) == NULL)
                            return false;
                    }
                    if (src->blob.data != NULL)
                    {
                        if ((tmp.blob.data = ::strdup(src->blob.data)) == NULL)
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

            param_t tmp;

            // Backup dst value to tmp
            switch (dst->flags & SF_TYPE_MASK)
            {
                case SF_TYPE_NONE: break;
                case SF_TYPE_I32: tmp.i32 = dst->i32; break;
                case SF_TYPE_U32: tmp.u32 = dst->u32; break;
                case SF_TYPE_I64: tmp.i64 = dst->i64; break;
                case SF_TYPE_U64: tmp.u64 = dst->u64; break;
                case SF_TYPE_F32: tmp.f32 = dst->f32; break;
                case SF_TYPE_F64: tmp.f64 = dst->f64; break;
                case SF_TYPE_STR: tmp.str = dst->str; break;
                case SF_TYPE_BLOB:
                    tmp.blob.length = dst->blob.length;
                    tmp.blob.ctype  = dst->blob.ctype;
                    tmp.blob.data   = dst->blob.data;
                    break;
                default:
                    break;
            }

            // Copy self value to dst
            switch (flags & SF_TYPE_MASK)
            {
                case SF_TYPE_NONE: break;
                case SF_TYPE_I32: dst->i32 = i32; break;
                case SF_TYPE_U32: dst->u32 = u32; break;
                case SF_TYPE_I64: dst->i64 = i64; break;
                case SF_TYPE_U64: dst->u64 = u64; break;
                case SF_TYPE_F32: dst->f32 = f32; break;
                case SF_TYPE_F64: dst->f64 = f64; break;
                case SF_TYPE_STR: dst->str = str; break;
                case SF_TYPE_BLOB:
                    dst->blob.length= blob.length;
                    dst->blob.ctype = blob.ctype;
                    dst->blob.data  = blob.data;
                    break;
                default:
                    break;
            }

            // Copy temp value to self
            switch (dst->flags & SF_TYPE_MASK)
            {
                case SF_TYPE_NONE: break;
                case SF_TYPE_I32: i32 = tmp.i32; break;
                case SF_TYPE_U32: u32 = tmp.u32; break;
                case SF_TYPE_I64: i64 = tmp.i64; break;
                case SF_TYPE_U64: u64 = tmp.u64; break;
                case SF_TYPE_F32: f32 = tmp.f32; break;
                case SF_TYPE_F64: f64 = tmp.f64; break;
                case SF_TYPE_STR: str = tmp.str; break;
                case SF_TYPE_BLOB:
                    blob.length     = tmp.blob.length;
                    blob.ctype      = tmp.blob.ctype;
                    blob.data       = tmp.blob.data;
                    break;
                default:
                    break;
            }

            // Swap flags last
            lsp::swap(flags, dst->flags);
        }

        void param_t::clear()
        {
            name.truncate();
            comment.truncate();

            switch (flags & SF_TYPE_MASK)
            {
                case SF_TYPE_STR:
                    if (str != NULL)
                        ::free(str);
                    str     = NULL;
                    break;
                case SF_TYPE_BLOB:
                    if (blob.ctype != NULL)
                        ::free(blob.ctype);
                    if (blob.data != NULL)
                        ::free(blob.data);
                    blob.length     = 0;
                    blob.ctype      = NULL;
                    blob.data       = NULL;
                    break;
            }

            flags = SF_TYPE_NONE;
        }
    }
}


