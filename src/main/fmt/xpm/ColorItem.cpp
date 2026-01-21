/*
 * Copyright (C) 2026 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2026 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-runtime-lib
 * Created on: 19 янв. 2026 г.
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

#include <lsp-plug.in/runtime/version.h>

#include <lsp-plug.in/fmt/xpm/ColorItem.h>
#include <lsp-plug.in/common/alloc.h>
#include <lsp-plug.in/stdlib/string.h>

namespace lsp
{
    namespace xpm
    {
        ColorItem::ColorItem()
        {
            sName       = NULL;
            nColor64    = 0xff000000;
            bWide       = false;
        }

        ColorItem::ColorItem(const ColorItem & src)
        {
            sName       = (src.sName != NULL) ? strdup(src.sName) : NULL;
            nColor64    = src.nColor64;
            bWide       = src.bWide;
        }

        ColorItem::ColorItem(ColorItem && src)
        {
            sName       = src.sName;
            nColor64    = src.nColor64;
            bWide       = src.bWide;

            src.sName   = NULL;
            src.nColor64= 0;
            src.bWide   = false;
        }

        ColorItem::ColorItem(const char *id)
        {
            sName       = strdup(id);
            nColor64    = 0xff000000;
            bWide       = false;
        }

        ColorItem::ColorItem(uint32_t value)
        {
            sName       = NULL;
            nColor64    = value;
            bWide       = false;
        }

        ColorItem::ColorItem(uint64_t value)
        {
            sName       = NULL;
            nColor64    = value;
            bWide       = true;
        }

        ColorItem::ColorItem(const char *id, uint32_t value)
        {
            sName       = strdup(id);
            nColor64    = value;
            bWide       = false;
        }

        ColorItem::ColorItem(const char *id, uint64_t value)
        {
            sName       = strdup(id);
            nColor64    = value;
            bWide       = true;
        }

        ColorItem::~ColorItem()
        {
            if (sName != NULL)
            {
                sName       = NULL;
                free(sName);
            }
        }

        void ColorItem::clear_name()
        {
            if (sName != NULL)
            {
                sName       = NULL;
                free(sName);
            }
        }

        bool ColorItem::set_name(const char *name)
        {
            if (name == NULL)
            {
                clear_name();
                return true;
            }

            char *str   = strdup(name);
            if (str == NULL)
                return false;

            if (sName != NULL)
                free(sName);
            sName       = str;

            return true;
        }

        bool ColorItem::set_name(const char *name, size_t len)
        {
            if (name == NULL)
            {
                clear_name();
                return true;
            }

            char *str   = static_cast<char *>(malloc(len + 1));
            if (str == NULL)
                return false;
            memcpy(str, name, len);
            str[len] = '\0';

            if (sName != NULL)
                free(sName);
            sName       = str;

            return true;
        }

        bool ColorItem::is_none() const noexcept
        {
            return (bWide) ? uint32_t(nColor64 >> 48) == 0xffff : (uint32_t(nColor64) >> 24) == 0xff;
        }

        uint32_t ColorItem::rgba32() const noexcept
        {
            if (!bWide)
                return uint32_t(nColor64);

            const uint64_t c = nColor64;
            return
                (uint32_t(c >> 32) & 0xff000000) |
                (uint32_t(c >> 24) & 0x00ff0000) |
                (uint32_t(c >> 16) & 0x0000ff00) |
                (uint32_t(c >> 8)  & 0x000000ff);
        }

        uint64_t ColorItem::rgba64() const noexcept
        {
            if (bWide)
                return nColor64;

            const uint64_t c = nColor64;
            return
                ((c & 0xff000000) << 32) |
                ((c & 0x00ff0000) << 24) |
                ((c & 0x0000ff00) << 16) |
                ((c & 0x000000ff) << 8);
        }

        void ColorItem::set_rgba32(uint32_t value)
        {
            nColor64    = value;
            bWide       = false;
        }

        void ColorItem::set_rgba64(uint64_t value)
        {
            nColor64    = value;
            bWide       = true;
        }

        void ColorItem::set_none()
        {
            nColor64    = 0xff000000;
            bWide       = false;
        }

        void ColorItem::swap(ColorItem & src)
        {
            lsp::swap(sName, src.sName);
            lsp::swap(nColor64, src.nColor64);
            lsp::swap(bWide, src.bWide);
        }

        void ColorItem::swap(ColorItem * src)
        {
            lsp::swap(sName, src->sName);
            lsp::swap(nColor64, src->nColor64);
            lsp::swap(bWide, src->bWide);
        }

    } /* namespace xpm */
} /* namespace lsp */


