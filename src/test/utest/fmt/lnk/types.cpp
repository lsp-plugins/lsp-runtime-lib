/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-runtime-lib
 * Created on: 11 авг. 2022 г.
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

#include <lsp-plug.in/common/types.h>
#include <lsp-plug.in/common/endian.h>
#include <lsp-plug.in/fmt/lnk/types.h>
#include <lsp-plug.in/stdlib/string.h>
#include <lsp-plug.in/test-fw/utest.h>


namespace lsp
{
    using namespace lsp::lnk;
} /* namespace lsp */

namespace
{
    using namespace lsp;

    static const uint8_t sample_file[] =
    {
        0x4C, 0x00, 0x00, 0x00, 0x01, 0x14, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC0, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x46, 0x9B, 0x00, 0x08, 0x00, 0x20, 0x00, 0x00, 0x00, 0xD0, 0xE9, 0xEE, 0xF2,
        0x15, 0x15, 0xC9, 0x01, 0xD0, 0xE9, 0xEE, 0xF2, 0x15, 0x15, 0xC9, 0x01, 0xD0, 0xE9, 0xEE, 0xF2,
        0x15, 0x15, 0xC9, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xBD, 0x00, 0x14, 0x00,
        0x1F, 0x50, 0xE0, 0x4F, 0xD0, 0x20, 0xEA, 0x3A, 0x69, 0x10, 0xA2, 0xD8, 0x08, 0x00, 0x2B, 0x30,
        0x30, 0x9D, 0x19, 0x00, 0x2F, 0x43, 0x3A, 0x5C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46, 0x00, 0x31, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x2C, 0x39, 0x69, 0xA3, 0x10, 0x00, 0x74, 0x65, 0x73, 0x74, 0x00, 0x00, 0x32,
        0x00, 0x07, 0x00, 0x04, 0x00, 0xEF, 0xBE, 0x2C, 0x39, 0x65, 0xA3, 0x2C, 0x39, 0x69, 0xA3, 0x26,
        0x00, 0x00, 0x00, 0x03, 0x1E, 0x00, 0x00, 0x00, 0x00, 0xF5, 0x1E, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x74, 0x00, 0x65, 0x00, 0x73, 0x00, 0x74, 0x00, 0x00, 0x00, 0x14,
        0x00, 0x48, 0x00, 0x32, 0x00, 0x00, 0x00, 0x00, 0x00, 0x2C, 0x39, 0x69, 0xA3, 0x20, 0x00, 0x61,
        0x2E, 0x74, 0x78, 0x74, 0x00, 0x34, 0x00, 0x07, 0x00, 0x04, 0x00, 0xEF, 0xBE, 0x2C, 0x39, 0x69,
        0xA3, 0x2C, 0x39, 0x69, 0xA3, 0x26, 0x00, 0x00, 0x00, 0x2D, 0x6E, 0x00, 0x00, 0x00, 0x00, 0x96,
        0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x61, 0x00, 0x2E, 0x00, 0x74,
        0x00, 0x78, 0x00, 0x74, 0x00, 0x00, 0x00, 0x14, 0x00, 0x00, 0x00, 0x3C, 0x00, 0x00, 0x00, 0x1C,
        0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x1C, 0x00, 0x00, 0x00, 0x2D, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x3B, 0x00, 0x00, 0x00, 0x11, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x81,
        0x8A, 0x7A, 0x30, 0x10, 0x00, 0x00, 0x00, 0x00, 0x43, 0x3A, 0x5C, 0x74, 0x65, 0x73, 0x74, 0x5C,
        0x61, 0x2E, 0x74, 0x78, 0x74, 0x00, 0x00, 0x07, 0x00, 0x2E, 0x00, 0x5C, 0x00, 0x61, 0x00, 0x2E,
        0x00, 0x74, 0x00, 0x78, 0x00, 0x74, 0x00, 0x07, 0x00, 0x43, 0x00, 0x3A, 0x00, 0x5C, 0x00, 0x74,
        0x00, 0x65, 0x00, 0x73, 0x00, 0x74, 0x00, 0x60, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0xA0, 0x58,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x63, 0x68, 0x72, 0x69, 0x73, 0x2D, 0x78, 0x70, 0x73,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x78, 0xC7, 0x94, 0x47, 0xFA, 0xC7, 0x46, 0xB3,
        0x56, 0x5C, 0x2D, 0xC6, 0xB6, 0xD1, 0x15, 0xEC, 0x46, 0xCD, 0x7B, 0x22, 0x7F, 0xDD, 0x11, 0x94,
        0x99, 0x00, 0x13, 0x72, 0x16, 0x87, 0x4A, 0x40, 0x78, 0xC7, 0x94, 0x47, 0xFA, 0xC7, 0x46, 0xB3,
        0x56, 0x5C, 0x2D, 0xC6, 0xB6, 0xD1, 0x15, 0xEC, 0x46, 0xCD, 0x7B, 0x22, 0x7F, 0xDD, 0x11, 0x94,
        0x99, 0x00, 0x13, 0x72, 0x16, 0x87, 0x4A, 0x00, 0x00, 0x00, 0x00,
    };

    static bool utf16_cmp(const void *ptr, const char *str)
    {
        const lsp_utf16_t *ustr = static_cast<const lsp_utf16_t *>(ptr);
        for (; *str != '\0'; ++str, ++ustr)
        {
            if (LE_TO_CPU(*ustr) != *str)
                return false;
        }
        return true;
    }
} /* namespace */

UTEST_BEGIN("runtime.fmt.lnk", types)

    void test_parse_sample_file()
    {
        const uint8_t *p        = sample_file;
        const uint8_t *e        = &p[sizeof(sample_file)];

        //-----------------------------------------------
        // Process mandatory header
        const shlink_header_t *hdr = reinterpret_cast<const shlink_header_t *>(p);
        p += sizeof(shlink_header_t);
        UTEST_ASSERT(p <= e);

        UTEST_ASSERT(LE_TO_CPU(hdr->size) == sizeof(shlink_header_t));

        UTEST_ASSERT(LE_TO_CPU(hdr->clsid.parts[0]) == SHLINK_FILE_HEADER_GUID.parts[0]);
        UTEST_ASSERT(LE_TO_CPU(hdr->clsid.parts[1]) == SHLINK_FILE_HEADER_GUID.parts[1]);
        UTEST_ASSERT(LE_TO_CPU(hdr->clsid.parts[2]) == SHLINK_FILE_HEADER_GUID.parts[2]);
        UTEST_ASSERT(LE_TO_CPU(hdr->clsid.parts[3]) == SHLINK_FILE_HEADER_GUID.parts[3]);

        UTEST_ASSERT(LE_TO_CPU(hdr->flags) ==
            (SHLINK_HAS_LINK_TARGET_IDLIST | SHLINK_HAS_LINK_INFO | SHLINK_HAS_RELATIVE_PATH |
            SHLINK_HAS_WORKING_DIR | SHLINK_IS_UNICODE | SHLINK_ENABLE_TARGET_METADATA));

        UTEST_ASSERT(LE_TO_CPU(hdr->file_attributes) == SHLINK_FILE_ATTRIBUTE_ARCHIVE);

        UTEST_ASSERT(LE_TO_CPU(hdr->creation_time.loword) == 0xf2eee9d0);
        UTEST_ASSERT(LE_TO_CPU(hdr->creation_time.hiword) == 0x01c91515);
        UTEST_ASSERT(LE_TO_CPU(hdr->access_time.loword) == 0xf2eee9d0);
        UTEST_ASSERT(LE_TO_CPU(hdr->access_time.hiword) == 0x01c91515);
        UTEST_ASSERT(LE_TO_CPU(hdr->write_time.loword) == 0xf2eee9d0);
        UTEST_ASSERT(LE_TO_CPU(hdr->write_time.hiword) == 0x01c91515);

        UTEST_ASSERT(LE_TO_CPU(hdr->file_size) == 0);
        UTEST_ASSERT(LE_TO_CPU(hdr->icon_index) == 0);
        UTEST_ASSERT(LE_TO_CPU(hdr->show_command) == SHLINK_SW_SHOWNORMAL);
        UTEST_ASSERT(LE_TO_CPU(hdr->hotkey) == 0);

        UTEST_ASSERT(LE_TO_CPU(hdr->reserved1) == 0);
        UTEST_ASSERT(LE_TO_CPU(hdr->reserved2) == 0);
        UTEST_ASSERT(LE_TO_CPU(hdr->reserved3) == 0);

        //-----------------------------------------------
        // Process link target idlist
        const shlink_idlist_t *idlist   = reinterpret_cast<const shlink_idlist_t *>(p);
        p += sizeof(shlink_idlist_t);
        UTEST_ASSERT(p <= e);

        UTEST_ASSERT(LE_TO_CPU(idlist->size) == 0x00BD);

        // Item # 1: "computer"
        const shlink_itemid_t *item = reinterpret_cast<const shlink_itemid_t *>(p);
        UTEST_ASSERT(p + sizeof(shlink_itemid_t) <= e);
        UTEST_ASSERT(LE_TO_CPU(item->size) == 0x0014);
        p += LE_TO_CPU(item->size);
        UTEST_ASSERT(p <= e);
        // TODO: check contents

        // Item # 2: "c:"
        item = reinterpret_cast<const shlink_itemid_t *>(p);
        UTEST_ASSERT(p + sizeof(shlink_itemid_t) <= e);
        UTEST_ASSERT(LE_TO_CPU(item->size) == 0x0019);
        p += LE_TO_CPU(item->size);
        UTEST_ASSERT(p <= e);
        // TODO: check contents

        // Item # 3: "test"
        item = reinterpret_cast<const shlink_itemid_t *>(p);
        UTEST_ASSERT(p + sizeof(shlink_itemid_t) <= e);
        UTEST_ASSERT(LE_TO_CPU(item->size) == 0x0046);
        p += LE_TO_CPU(item->size);
        UTEST_ASSERT(p <= e);
        // TODO: check contents

        // Item # 4: "a.txt"
        item = reinterpret_cast<const shlink_itemid_t *>(p);
        UTEST_ASSERT(p + sizeof(shlink_itemid_t) <= e);
        UTEST_ASSERT(LE_TO_CPU(item->size) == 0x0048);
        p += LE_TO_CPU(item->size);
        UTEST_ASSERT(p <= e);
        // TODO: check contents

        // TerminalID
        item = reinterpret_cast<const shlink_itemid_t *>(p);
        UTEST_ASSERT(p + sizeof(shlink_itemid_t) <= e);
        UTEST_ASSERT(LE_TO_CPU(item->size) == 0);
        p += sizeof(shlink_itemid_t);

        //-----------------------------------------------
        // Process LinkInfo structure
        const shlink_link_info_t *linfo = reinterpret_cast<const shlink_link_info_t *>(p);
        UTEST_ASSERT(p + sizeof(shlink_link_info_t) <= e);

        UTEST_ASSERT(LE_TO_CPU(linfo->size) == 0x0000003C);
        UTEST_ASSERT(p + LE_TO_CPU(linfo->size) <= e);

        UTEST_ASSERT(LE_TO_CPU(linfo->header_size) == sizeof(shlink_link_info_t) - sizeof(uint32_t));
        UTEST_ASSERT(LE_TO_CPU(linfo->flags) == SHLINK_LINK_INFO_VOLUME_ID_AND_LOCAL_BASE_PATH);
        UTEST_ASSERT(LE_TO_CPU(linfo->volume_id_offset) == 0x0000001C);

        // Verify the volume offset and volume information
        const shlink_volume_id_t *vid   = reinterpret_cast<const shlink_volume_id_t *>(p + LE_TO_CPU(linfo->volume_id_offset));
        UTEST_ASSERT(p + LE_TO_CPU(linfo->volume_id_offset) + sizeof(shlink_volume_id_t) <= e);
        UTEST_ASSERT(LE_TO_CPU(vid->size) == 0x00000011);
        UTEST_ASSERT(p + LE_TO_CPU(linfo->volume_id_offset) + LE_TO_CPU(vid->size) <= e);
        UTEST_ASSERT(LE_TO_CPU(vid->drive_type) == SHLINK_DRIVE_FIXED);
        UTEST_ASSERT(LE_TO_CPU(vid->drive_serial) == 0x307A8A81);
        UTEST_ASSERT(LE_TO_CPU(vid->volume_label_offset) == 0x00000010);
        UTEST_ASSERT(LE_TO_CPU(vid->data[0]) == 0);

        // Verify LocalBasePathOffset and LocalBasePath
        UTEST_ASSERT(LE_TO_CPU(linfo->local_base_path_offset) == 0x002d);
        UTEST_ASSERT(p + LE_TO_CPU(linfo->local_base_path_offset) <= e);
        const char *s = reinterpret_cast<const char *>(p + LE_TO_CPU(linfo->local_base_path_offset));
        UTEST_ASSERT(strcmp(s, "C:\\test\\a.txt") == 0);

        UTEST_ASSERT(LE_TO_CPU(linfo->common_network_relative_link_offset) == 0);

        UTEST_ASSERT(LE_TO_CPU(linfo->common_path_suffix_offset) == 0x003b);
        UTEST_ASSERT(p + LE_TO_CPU(linfo->common_path_suffix_offset) <= e);
        s = reinterpret_cast<const char *>(p + LE_TO_CPU(linfo->common_path_suffix_offset));
        UTEST_ASSERT(strcmp(s, "") == 0);

        // Move the pointer
        p += LE_TO_CPU(linfo->size);
        UTEST_ASSERT(p <= e);

        //-----------------------------------------------
        // Process RELATIVE_PATH string data
        const shlink_string_data_t *sdata   = reinterpret_cast<const shlink_string_data_t *>(p);
        p += sizeof(shlink_string_data_t);
        UTEST_ASSERT(p <= e);
        UTEST_ASSERT(LE_TO_CPU(sdata->length) == 0x0007);
        p += sizeof(lsp_utf16_t) * LE_TO_CPU(sdata->length);
        UTEST_ASSERT(p <= e);
        UTEST_ASSERT(utf16_cmp(sdata->data, ".\\a.txt") == true);

        //-----------------------------------------------
        // Process WORKING_DIR string data
        sdata   = reinterpret_cast<const shlink_string_data_t *>(p);
        p += sizeof(shlink_string_data_t);
        UTEST_ASSERT(p <= e);
        UTEST_ASSERT(LE_TO_CPU(sdata->length) == 0x0007);
        p += sizeof(lsp_utf16_t) * LE_TO_CPU(sdata->length);
        UTEST_ASSERT(p <= e);
        UTEST_ASSERT(utf16_cmp(sdata->data, "C:\\test") == true);

        //-----------------------------------------------
        // Process EXTRA_DATA structure
        // Tracker data block
        const shlink_tracker_data_block_t *tr = reinterpret_cast<const shlink_tracker_data_block_t *>(p);
        p += sizeof(shlink_tracker_data_block_t);
        UTEST_ASSERT(p <= e);

        UTEST_ASSERT(LE_TO_CPU(tr->size) == sizeof(shlink_tracker_data_block_t));
        UTEST_ASSERT(LE_TO_CPU(tr->signature) == SHLINK_TRACKER_DATA_BLOCK);
        UTEST_ASSERT(LE_TO_CPU(tr->length) == 0x00000058);
        UTEST_ASSERT(LE_TO_CPU(tr->version) == 0x00000000);
        UTEST_ASSERT(strcmp(tr->machine_id, "chris-xps") == 0);

        // Terminal block
        const uint32_t *terminal = reinterpret_cast<const uint32_t *>(p);
        p += sizeof(uint32_t);
        UTEST_ASSERT(p == e);
        UTEST_ASSERT(LE_TO_CPU(*terminal) == 0);
    }


    UTEST_MAIN
    {
        test_parse_sample_file();
    }

UTEST_END




