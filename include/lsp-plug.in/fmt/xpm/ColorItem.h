/*
 * Copyright (C) 2026 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2026 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-runtime-lib
 * Created on: 18 янв. 2026 г.
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

#ifndef LSP_PLUG_IN_FMT_XPM_COLORITEM_H_
#define LSP_PLUG_IN_FMT_XPM_COLORITEM_H_

#include <lsp-plug.in/runtime/version.h>

#include <lsp-plug.in/common/types.h>
#include <lsp-plug.in/fmt/xpm/CString.h>

namespace lsp
{
    namespace xpm
    {
        enum color_class_t
        {
            CC_NONE,
            CC_RGB24,
            CC_RGB48,
            CC_ALIAS
        };

        /**
         * XPM color item
         */
        class ColorItem
        {
            private:
                CString sID;            // ID of color
                CString sAlias;         // Color name
                color_class_t enClass;  // Value class
                union
                {
                    uint32_t    nRGB24;     // RGB24 value
                    uint64_t    nRGB48;     // RGB48 value
                };

            public:
                ColorItem();
                ColorItem(const ColorItem & src);
                ColorItem(ColorItem && src);
                explicit ColorItem(const char *id);
                ColorItem(const char *id, const char *value);
                ColorItem(const char *id, uint32_t value);
                ColorItem(const char *id, uint64_t value);

            public:
                inline bool is_none() const noexcept        { return enClass == CC_NONE;    }
                inline bool is_rgb24() const noexcept       { return enClass == CC_RGB24;   }
                inline bool is_rgb48() const noexcept       { return enClass == CC_RGB48;   }
                inline bool is_alias() const noexcept       { return enClass == CC_ALIAS;   }

                inline const char *id() const noexcept      { return sID.get();             }
                inline const char *alias() const noexcept   { return (enClass == CC_ALIAS) ? sAlias.get() : NULL;   }
                uint32_t rgb24() const noexcept;
                uint64_t rgb48() const noexcept;

                inline bool set_id(const char *id)          { return sID.set(id);           }
                inline bool set_id(CStringBuffer & buf, size_t offset)  { return sID.set(buf, offset);  }

                bool set_alias(const char *value);
                bool set_alias(CStringBuffer & buf, size_t offset);

                void set_rgb24(uint32_t value);
                void set_rgb48(uint64_t value);
                void set_none();

                void swap(ColorItem & src);
                void swap(ColorItem * src);

        };

    } /* namespace xpm */
} /* namespace lsp */



#endif /* LSP_PLUG_IN_FMT_XPM_COLORITEM_H_ */
