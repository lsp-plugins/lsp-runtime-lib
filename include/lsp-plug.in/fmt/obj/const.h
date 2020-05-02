/*
 * const.h
 *
 *  Created on: 21 апр. 2020 г.
 *      Author: sadko
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
