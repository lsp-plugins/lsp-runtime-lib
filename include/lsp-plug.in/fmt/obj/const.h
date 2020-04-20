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
            EV_NORMAL,      ///< New normal coordinates
            EV_TEXCOORD,    ///< New textrue coordinates
        };

        typedef struct event_t
        {
            event_type_t type;

            union
            {
                struct { float x, y, z;     } vertex;
                struct { float dx, dy, dz;  } normal;
                struct { float u, v;        } texcoord;
            };
            LSPString name;

            lltl::darray<size_t>    ivertex;
            lltl::darray<size_t>    inormal;
            lltl::darray<size_t>    itexcoord;
        } event_t;
    }
}


#endif /* LSP_PLUG_IN_FMT_OBJ_CONST_H_ */
