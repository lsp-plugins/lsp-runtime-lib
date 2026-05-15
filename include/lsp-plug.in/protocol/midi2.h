/*
 * Copyright (C) 2026 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2026 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-runtime-lib
 * Created on: 8 мая 2026 г.
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

#ifndef LSP_PLUG_IN_PROTOCOL_MIDI2_H_
#define LSP_PLUG_IN_PROTOCOL_MIDI2_H_

#include <lsp-plug.in/runtime/version.h>
#include <lsp-plug.in/common/types.h>

#include <lsp-plug.in/protocol/midi.h>
#include <lsp-plug.in/protocol/midi2/channel.h>
#include <lsp-plug.in/protocol/midi2/ump.h>
#include <lsp-plug.in/protocol/midi2/utility.h>

namespace lsp
{
    namespace midi2
    {
        enum message_t
        {
            MIDI2_MSG_UTILITY               = 0x0,
            MIDI2_MSG_SYSTEM_RT             = 0x1,
            MIDI2_MSG_SYSTEM_COMMON         = 0x1,
            MIDI1_MSG_CHANNEL_VOICE         = 0x2,
            MIDI2_MSG_DATA_64               = 0x3,
            MIDI2_MSG_SYSTEM_EXCLUSIVE      = 0x3,
            MIDI2_MSG_CHANNEL_VOICE         = 0x4,
            MIDI2_MSG_DATA_128              = 0x5,
            MIDI2_MSG_FLEX_DATA             = 0xd,
            MIDI2_MSG_UMP_STREAM            = 0xf,
        };

        typedef struct event_t
        {
            uint32_t        timestamp;      // Timestamp
            uint8_t         type;           // Type of the MIDI event
            uint8_t         group;          // Group (if present)
            uint8_t         channel;        // Channel (if present)
            uint8_t         status;         // Status (if present)

            union
            {
                // MIDI2_MSG_CHANNEL_VOICE
                struct {
                    uint16_t            index;          // Index
                } ch_voice;

            };

            // Some structured data
            union
            {
                // MIDI2_MSG_UTILITY
                utility::message_t      utility;
                // MIDI2_MSG_UMP_STREAM
                ump::message_t          ump;
                // MIDI1_MSG_CHANNEL_VOICE
                midi::payload_t         midi1;
                // MIDI2_MSG_CHANNEL_VOICE
                channel::message_t      voice;

                // Some payload data (for 32-bit alignment)
                uint32_t                payload;

                // Other data (big-endian format)
                uint8_t                 data[16];
            };
        } event_t;

    } /* namespace midi2 */
} /* namespace lsp */


#endif /* LSP_PLUG_IN_PROTOCOL_MIDI2_H_ */
