/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-runtime-lib
 * Created on: 21 апр. 2020 г.
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

#ifndef LSP_PLUG_IN_FMT_OBJ_CONST_H_
#define LSP_PLUG_IN_FMT_OBJ_CONST_H_

#include <lsp-plug.in/runtime/version.h>
#include <lsp-plug.in/common/types.h>
#include <lsp-plug.in/common/status.h>

#include <lsp-plug.in/lltl/darray.h>

namespace lsp
{
    namespace obj
    {
        enum event_type_t
        {
            EV_NONE,        ///< No event, some error occurred
            EV_OBJECT,      ///< Start of the new object
            EV_VERTEX,      ///< New vertex coordinates
            EV_PVERTEX,     ///< New parametrized vertex coordinates
            EV_NORMAL,      ///< New normal coordinates
            EV_TEXCOORD,    ///< New textrue coordinates
            EV_FACE,        ///< Face event
            EV_LINE,        ///< Line event
            EV_POINT,       ///< Point event
        };

        typedef ssize_t     index_t;

        typedef struct event_t
        {
            event_type_t type;

            union
            {
                struct { float x, y, z, w;      } vertex;
                struct { float dx, dy, dz, dw;  } normal;
                struct { float u, v, w;         } texcoord;
            };
            LSPString name;

            lltl::darray<index_t>   ivertex;        // Indexes of vertices
            lltl::darray<index_t>   inormal;        // Indexes of normals
            lltl::darray<index_t>   itexcoord;      // Indexes of texture coordinates
        } event_t;
    }
}


#endif /* LSP_PLUG_IN_FMT_OBJ_CONST_H_ */
