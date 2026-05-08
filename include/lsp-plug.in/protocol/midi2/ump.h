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

#ifndef LSP_PLUG_IN_PROTOCOL_MIDI2_UMP_H_
#define LSP_PLUG_IN_PROTOCOL_MIDI2_UMP_H_

#include <lsp-plug.in/runtime/version.h>
#include <lsp-plug.in/common/types.h>

namespace lsp
{
    namespace midi2
    {
        namespace ump
        {
            enum status_t
            {
                DISCOVERY                           = 0x00,
                ENDPOINT_INFO                       = 0x01,
                DEVICE_IDENTIFY                     = 0x02,
                ENDPOINT_NAME                       = 0x03,
                PRODUCT_INSTANCE_IDENTIFY           = 0x04,
                STREAM_CONFIG_REQUEST               = 0x05,
                STREAM_CONFIG_NOTIFY                = 0x06,
                FUNCION_BLOCK_DISCOVERY             = 0x10,
                FUNCION_BLOCK_INFO_NOTIFY           = 0x11,
                FUNCION_BLOCK_NAME_NOTIFY           = 0x12,
                FUNCION_BLOCK_START_CLIP            = 0x20,
                FUNCION_BLOCK_END_CLIP              = 0x21,
            };

            enum format_t
            {
                FORMAT_COMPLETE                     = 0x00,
                FORMAT_START                        = 0x01,
                FORMAT_CONTINUE                     = 0x02,
                FORMAT_END                          = 0x03,
            };

            enum discovery_flags_t
            {
                DISCOVERY_ENDPOINT_INFO             = 1 << 0,
                DISCOVERY_DEVICE_IDENTIFY           = 1 << 1,
                DISCOVERY_ENDPOINT_NAME             = 1 << 2,
                DISCOVERY_PRODUCT_INSTANCE          = 1 << 3,
                DISCOVERY_STREAM_CONFIG             = 1 << 4,
            };

            enum endpoint_flags_t
            {
                ENDPOINT_RXJR_CAP                   = 1 << 0,   // Receive JR timestamp capability
                ENDPOINT_TXJR_CAP                   = 1 << 1,   // Transmit JR timestamp capability
                ENDPOINT_MIDI1_CAP                  = 1 << 2,   // MIDI 1.0 protocol capability
                ENDPOINT_MIDI2_CAP                  = 1 << 3,   // MIDI 2.0 protocol capability
                ENDPOINT_STATIC_BLOCKS              = 1 << 4,   // Static function blocks
            };

            enum stream_config_protocol_t
            {
                STREAM_CONFIG_MIDI1                 = 0x01,     // MIDI 1.0 protocol
                STREAM_CONFIG_MIDI2                 = 0x02,     // MIDI 2.0 protocol
            };

            enum stream_config_flags_t
            {
                STREAM_CONFIG_RXJR                  = 1 << 0,   // Receive JR timestamp
                STREAM_CONFIG_TXJR                  = 1 << 1,   // Transmit JR timestamp
            };

            enum function_block_discovery_filter_t
            {
                FUNCTION_BLOCK_INFO_REQ             = 1 << 0,   // Request function block info notification
                FUNCTION_BLOCK_NAME_REQ             = 1 << 1,   // Request function block name notification
            };

            enum function_block_info_notify_flags_t
            {
                FUNCTION_BLOCK_INFO_NOTIFY_MIDI_RX  = 1 << 0,   // Function block receives MIDI messages
                FUNCTION_BLOCK_INFO_NOTIFY_MIDI_TX  = 1 << 1,   // Function block transmits MIDI messages
                FUNCTION_BLOCK_INFO_NOTIFY_UI_RX    = 1 << 2,   // Primarily receiver or destination of MIDI messages
                FUNCTION_BLOCK_INFO_NOTIFY_UI_TX    = 1 << 3,   // Primarily sender or source of MIDI messages
                FUNCTION_BLOCK_INFO_NOTIFY_ACTIVE   = 1 << 4,   // Function block is active
            };

            enum function_block_info_notify_midi1_t
            {
                FUNCTION_BLOCK_INFO_NOTIFY_NOT_MIDI1    = 0x00,   // Not MIDI1
                FUNCTION_BLOCK_INFO_NOTIFY_NO_RESTRICT  = 0x01,   // MIDI1, do not restrict bandwidth
                FUNCTION_BLOCK_INFO_NOTIFY_RESTRICT     = 0x02,   // MIDI1, restrict bandwidth
            };

            typedef struct discovery_t
            {
                uint8_t         version_major;      // Version major
                uint8_t         version_minor;      // Version minor
                uint8_t         filter_bitmap;      // Filter bitmap, @see discovery_flags_t
            } discovery_t;

            typedef struct endpoint_info_t
            {
                uint8_t         version_major;      // Version major
                uint8_t         version_minor;      // Version minor
                uint8_t         flags;              // Endpoint flags, @see endpoint_flags_t
                uint8_t         blocks;             // Number of function blocks
            } endpoint_info_t;

            typedef struct device_identify_t
            {
                uint32_t        manufactrurer;      // Device manufacturer
                uint16_t        family;             // Device family
                uint16_t        model;              // Device family model
                uint32_t        revision;           // Software revision level
            } device_identify_t;

            typedef struct endpoint_name_t
            {
                uint8_t         name[14];           // Endpoint name data
            } endpoint_name_t;

            typedef struct product_instance_identify_t
            {
                uint8_t         instance_id[14];    // Instance identifier
            } product_instance_identify_t;

            typedef struct stream_config_request_t
            {
                uint8_t         protocol;           // Protocol, @see stream_config_protocol_t
                uint8_t         flags;              // Flags, @see stream_config_flags_t
            } stream_config_request_t;

            typedef struct stream_config_notify_t
            {
                uint8_t         protocol;           // Protocol, @see stream_config_protocol_t
                uint8_t         flags;              // Flags, @see stream_config_flags_t
            } stream_config_notify_t;

            typedef struct function_block_discovery_t
            {
                uint8_t         block;              // Block number [0x00-0x1f, 0xff]
                uint8_t         filter;             // Flags, @see stream_config_flags_t
            } function_block_discovery_t;

            typedef struct function_block_info_notify_t
            {
                uint8_t         block;              // Block number [0x00-0x1f]
                uint8_t         first_group;        // First group [0x00-0x0f]
                uint8_t         groups_spanned;     // Number of groups spanned [1-16]
                uint8_t         midi_ci_format;     // MIDI CI Version/Format
                uint8_t         max_sysex8;         // maximum number of sysex8 streams
                uint8_t         flags;              // Flags, @see function_block_info_notify_flags_t
                uint8_t         midi1;              // MIDI 1 configuration, @see function_block_info_notify_midi1_t
            } function_block_info_notify_t;

            typedef struct function_block_name_notify_t
            {
                uint8_t         name[14];           // Function block name data
            } function_block_name_notify_t;

            typedef struct message_t
            {
                union
                {
                    // Discovery data (status == DISCOVERY)
                    discovery_t                     discovery;
                    // Endpoint info (status == ENDPOINT_INFO)
                    endpoint_info_t                 endpoint_info;
                    // Device identify (status == DEVICE_IDENTIFY)
                    device_identify_t               device_id;
                    // Endpoint name (status == ENDPOINT_NAME)
                    endpoint_name_t                 endpoint_name;
                    // Instance identify (status == PRODUCT_INSTANCE_IDENTIFY)
                    product_instance_identify_t     product_instance;
                    // Stream configuration request (status == STREAM_CONFIG_REQUEST)
                    stream_config_request_t         stream_cfg_request;
                    // Stream configuration notification (status == STREAM_CONFIG_NOTIFY)
                    stream_config_notify_t          stream_cfg_notify;
                    // Stream function block discovery (status == FUNCTION_BLOCK_DISCOVERY)
                    function_block_discovery_t      func_blk_discovery;
                    // Stream function block info notify (status == FUNCION_BLOCK_INFO_NOTIFY)
                    function_block_info_notify_t    func_blk_info_notify;
                    // Stream function block name notify (status == FUNCION_BLOCK_INFO_NOTIFY)
                    function_block_name_notify_t    func_blk_name_notify;
                };
            } message_t;
        } /* namespace ump */

    } /* namespace midi2 */
} /* namespace lsp */

#endif /* LSP_PLUG_IN_PROTOCOL_MIDI2_UMP_H_ */
