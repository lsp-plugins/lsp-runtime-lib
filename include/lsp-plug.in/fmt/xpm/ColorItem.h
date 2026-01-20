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
        /**
         * XPM color item
         */
        class ColorItem
        {
            private:
                char       *sName;          // Name of color
                uint64_t    nColor64;
                bool        bWide;          // Wide color

            public:
                ColorItem();
                ColorItem(const ColorItem & src);
                ColorItem(ColorItem && src);
                explicit ColorItem(const char *name);
                ColorItem(uint32_t value);
                ColorItem(uint64_t value);
                ColorItem(const char *name, uint32_t value);
                ColorItem(const char *name, uint64_t value);
                ~ColorItem();

            public:
                inline const char *name() const noexcept                    { return sName;                         }
                bool set_name(const char *name);
                bool set_name(const char *name, size_t len);
                void clear_name();

                inline bool is_wide() const noexcept            { return bWide;                         }
                bool is_none() const noexcept;
                uint32_t rgba32() const noexcept;
                uint64_t rgba64() const noexcept;

                void set_rgba32(uint32_t value);
                void set_rgba64(uint64_t value);
                void set_none();

                void swap(ColorItem & src);
                void swap(ColorItem * src);

        };

    } /* namespace xpm */
} /* namespace lsp */



#endif /* LSP_PLUG_IN_FMT_XPM_COLORITEM_H_ */
