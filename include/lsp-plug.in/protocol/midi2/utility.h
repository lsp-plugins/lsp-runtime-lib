/*
 * Copyright (C) 2026 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2026 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-runtime-lib
 * Created on: 9 мая 2026 г.
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

#ifndef LSP_PLUG_IN_PROTOCOL_MIDI2_UTILITY_H_
#define LSP_PLUG_IN_PROTOCOL_MIDI2_UTILITY_H_

#include <lsp-plug.in/runtime/version.h>
#include <lsp-plug.in/common/types.h>

namespace lsp
{
    namespace midi2
    {
        namespace utility
        {
            enum status_t
            {
                NOOP                         = 0x00,
                JITTER_REDUCTION_CLOCK       = 0x01,     // Jitter reduction clock message
                JITTER_REDUCTION_TIMESTAMP   = 0x02,     // Jitter reduction timestamp message
                DELTA_CLOCKSTAMP_TPQN        = 0x03,     // Delta clockstamp ticks per quarter note
                DELTA_CLOCKSTAMP_TSLE        = 0x04,     // Delta clockstamp ticks since last event
            };

            typedef struct jitter_reduction_clock_t
            {
                uint16_t                    clock;              // Sender clock time
            } jitter_reduction_clock_t;

            typedef struct jitter_reduction_timestamp_t
            {
                uint16_t                    timestamp;          // Sender clock timestamp
            } jitter_reduction_timestamp_t;

            typedef struct delta_clockstamp_tpqn_t
            {
                uint16_t                    ticks;              // Number of ticks per quarter note
            } delta_clockstamp_tpqn_t;

            typedef struct delta_clockstamp_tsle_t
            {
                uint16_t                    ticks;              // Number of ticks since last event
            } delta_clockstamp_tsle_t;

            typedef struct message_t
            {
                union
                {
                    // Jitter reduction clock (status == JITTER_REDUCTION_CLOCK)
                    jitter_reduction_clock_t        jr_clock;
                    // Jitter reduction timestamp (status == JITTER_REDUCTION_TIMESTAMP)
                    jitter_reduction_timestamp_t    jr_timestamp;
                    // Delta clockstamp ticks per quarter note (status == DELTA_CLOCKSTAMP_TPQN)
                    delta_clockstamp_tpqn_t         delta_tpqn;
                    // Delta clockstamp ticks since last event (status == DELTA_CLOCKSTAMP_TSLE)
                    delta_clockstamp_tsle_t         delta_tsle;
                };
            } message_t;

        } /* namespace utility */
    } /* namespace midi2 */
} /* namespace lsp */



#endif /* LSP_PLUG_IN_PROTOCOL_MIDI2_UTILITY_H_ */
