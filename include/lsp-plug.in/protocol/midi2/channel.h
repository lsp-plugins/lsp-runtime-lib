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

#ifndef LSP_PLUG_IN_PROTOCOL_MIDI2_CHANNEL_H_
#define LSP_PLUG_IN_PROTOCOL_MIDI2_CHANNEL_H_

#include <lsp-plug.in/runtime/version.h>
#include <lsp-plug.in/common/types.h>

namespace lsp
{
    namespace midi2
    {
        namespace channel
        {
            enum status_t
            {
                REGISTER_PER_NOTE           = 0x00,     // Register Per Note
                ASSIGNABLE_PER_NOTE         = 0x01,     // Assignable Per Note message
                REGISTER_CONTROLLER         = 0x02,     // Register Controllter
                ASSIGNABLE_CONTROLLER       = 0x03,     // Assignable Controller
                REL_REGISTER_CONTROLLER     = 0x04,     // Relative Register Controllter
                REL_ASSIGNABLE_CONTROLLER   = 0x05,     // Relative Assignable Controller
                PER_NOTE_PITCH_BEND         = 0x06,     // Per-Note Pitch Bend
                NOTE_OFF                    = 0x08,     // Note On
                NOTE_ON                     = 0x09,     // Note Off
                POLY_PRESSURE               = 0x0a,     // Poly Pressure (Aftertouch)
                CONTROL_CHANGE              = 0x0b,     // Control change
                PROGRAM_CHANGE              = 0x0c,     // Program change
                CHANNEL_PRESSOURE           = 0x0d,     // Channel pressure (Channel Aftertouch)
                PITCH_BEND                  = 0x0e,     // Pitch bend
                PER_NOTE_MANAGEMENT         = 0x0f,     // Per-Note management
            };

            enum note_attribute_type_t
            {
                NOTE_ATTRIBUTE_NO_DATA      = 0x00,     // No note attribute data
                NOTE_ATTRIBUTE_SPECIFIC     = 0x01,     // Manufacturer-specific data
                NOTE_ATTRIBUTE_PROFILE      = 0x02,     // Profile-specific data
                NOTE_ATTRIBUTE_PITCH        = 0x03,     // Pitch
            };

            enum per_note_management_flags_t
            {
                PER_NOTE_MANAGEMENT_RESET   = 1 << 0,   // Reset Per-Note controllers to default values
                PER_NOTE_MANAGEMENT_DETACH  = 1 << 0,   // Detach Per-Note controllers from previously received notes
            };

            enum program_change_flags_t
            {
                PROGRAM_CHANGE_BANK_VALID   = 1 << 0,   // Bank valid
            };

            typedef struct note_message_t
            {
                uint8_t         att_type;           // Attribute type, @see note_attribute_type_t
                uint16_t        velocity;           // Velocity
                uint16_t        attribute;          // Attribute value
            } note_message_t;

            typedef struct poly_pressure_t
            {
                uint32_t        data;               // Data
            } poly_pressure;

            typedef struct channel_pressure_t
            {
                uint32_t        data;               // Data
            } channel_pressure;

            typedef struct pitch_bend_t
            {
                uint32_t        bend;               // Pitch bend, 7.25 fixed-point centered at 0x80000000
            } pitch_bend_t;

            typedef struct per_note_pitch_bend_t
            {
                uint32_t        bend;               // Pitch bend, centered at 0x80000000
            } per_note_pitch_bend_t;

            typedef struct per_note_t
            {
                uint8_t         index;              // Index
                uint32_t        data;               // Data
            } per_note_t;

            typedef struct per_note_management_t
            {
                uint8_t         flags;              // Flags @see per_note_management_flags_t
            } per_note_management_t;

            typedef struct control_change_t
            {
                uint8_t         index;              // Index
                union
                {
                    uint8_t         source_note;        // For portamento CC (0x54)
                    uint8_t         num_channels;       // For Omin-Off/Mono message CC (0x7e)
                };
            } control_change_t;

            typedef struct program_change_t
            {
                uint8_t         flags;              // Flags, @see program_change_flags
                uint8_t         program;            // Program
                uint16_t        bank;               // Bank
            } program_change_t;

            typedef struct controller_t
            {
                uint8_t         bank;
                uint8_t         index;
                union
                {
                    // Pitch bend range
                    struct {
                        uint8_t     semitones;
                        uint8_t     cents;
                    } pitch;

                    // Coarse tuning
                    uint8_t         coarse;

                    // Tuning program change
                    uint8_t         tuning_program;

                    // Tuning bank select
                    uint8_t         tuning_bank;

                    // MPE MCM
                    uint8_t         num_channels;

                    // Other data
                    uint32_t        data;
                    uint8_t         bytes[4];
                };
            } controller_t;

            typedef struct rel_controller_t
            {
                uint8_t         bank;
                uint8_t         index;
                int32_t         data;
            } rel_controller_t;

            typedef struct message_t
            {
                uint8_t         note;               // Note number [0-127] (if present)
                union
                {
                    // Register Per Note/Assignable Per Note message (status == REGISTER_PER_NOTE || status == ASSIGNABLE_PER_NOTE)
                    per_note_t              per_note;
                    // Register Controllter/Assignable Controller message (status == REGISTER_CONTROLLER || status == ASSIGNABLE_CONTROLLER)
                    controller_t            controller;
                    // Relative egister Controllter/Relative Assignable Controller message (status == REL_REGISTER_CONTROLLER || status == REL_ASSIGNABLE_CONTROLLER)
                    rel_controller_t        rel_controller;
                    // Note On/Note Off message (status == NOTE_OFF || status == NOTE_ON)
                    note_message_t          note_ev;
                    // Poly pressure message (status == POLY_PRESSURE)
                    poly_pressure_t         aftertouch;
                    // Control change (status == CONTROL_CHANGE)
                    control_change_t        ctl_change;
                    // Channel pressure message (status == CHANNEL_PRESSURE)
                    channel_pressure_t      ch_aftertouch;
                    // Pitch bend message (status == PITCH_BEND)
                    pitch_bend_t            pitch;
                    // Per Note Management (status == PER_NOTE_MANAGEMENT)
                    per_note_management_t   per_note_manage;
                };
            } message_t;

        } /* namespace channel */
    } /* namespace midi2 */
} /* namespace lsp */



#endif /* LSP_PLUG_IN_PROTOCOL_MIDI2_CHANNEL_H_ */
