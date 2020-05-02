/*
 * debug.h
 *
 *  Created on: 29 мая 2019 г.
 *      Author: sadko
 */

#ifndef LSP_PLUG_IN_PROTOCOL_OSC_DEBUG_H_
#define LSP_PLUG_IN_PROTOCOL_OSC_DEBUG_H_

#include <lsp-plug.in/runtime/version.h>
#include <lsp-plug.in/protocol/osc/types.h>
#include <lsp-plug.in/common/debug.h>

namespace lsp
{
    namespace osc
    {
    #ifdef LSP_TRACE
        void dump_packet(const packet_t *packet);

        void dump_packet(const void *data, size_t size);
    #else
        inline void dump_packet(const packet_t *packet) {}

        inline void dump_packet(const void *data, size_t size) {}
    #endif
    }
}


#endif /* LSP_PLUG_IN_PROTOCOL_OSC_DEBUG_H_ */
