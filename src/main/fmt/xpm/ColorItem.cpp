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

namespace lsp
{
    namespace xpm
    {
        ColorItem::ColorItem():
            enClass(CC_NONE)
        {
            nRGB48      = 0;
        }

        ColorItem::ColorItem(const ColorItem & src):
            sID(src.sID),
            sAlias(src.sAlias),
            enClass(src.enClass),
            nRGB48(src.nRGB48)
        {
        }

        ColorItem::ColorItem(ColorItem && src):
            sID(lsp::move(src.sID)),
            sAlias(lsp::move(src.sAlias)),
            enClass(src.enClass),
            nRGB48(src.nRGB48)
        {
        }

        ColorItem::ColorItem(const char *id):
            sID(id),
            enClass(CC_NONE)
        {
            nRGB48      = 0;
        }

        ColorItem::ColorItem(const char *id, const char *value):
            sID(id),
            sAlias(value),
            enClass(CC_ALIAS)
        {
            nRGB48      = 0;
        }

        ColorItem::ColorItem(const char *id, uint32_t value):
            sID(id),
            enClass(CC_RGB24)
        {
            nRGB48      = 0;
            nRGB24      = value;
        }

        ColorItem::ColorItem(const char *id, uint64_t value):
            sID(id),
            enClass(CC_RGB48)
        {
            nRGB48      = value;
        }

        uint32_t ColorItem::rgb24() const noexcept
        {
            switch (enClass)
            {
                case CC_RGB24:
                    return nRGB24;
                case CC_RGB48:
                {
                    const uint32_t c = nRGB24;
                    return
                        (uint64_t(c & 0xff0000) << 24) |
                        (uint64_t(c & 0x00ff00) << 16) |
                        (uint64_t(c & 0x0000ff) << 8);
                }
                default:
                    break;
            }
            return 0;
        }

        uint64_t ColorItem::rgb48() const noexcept
        {
            switch (enClass)
            {
                case CC_RGB48:
                    return nRGB48;
                case CC_RGB24:
                {
                    const uint64_t c = nRGB48;
                    return
                        (uint32_t(c >> 24) & 0xff0000) |
                        (uint32_t(c >> 16) & 0x00ff00) |
                        (uint32_t(c >> 8)  & 0x0000ff);
                }
                default:
                    break;
            }
            return 0;
        }

        bool ColorItem::set_alias(const char *value)
        {
            if (!sAlias.set(value))
                return false;

            enClass = CC_ALIAS;
            return true;
        }

        bool ColorItem::set_alias(CStringBuffer & buf, size_t offset)
        {
            if (!sAlias.set(buf, offset))
                return false;

            enClass = CC_ALIAS;
            return true;
        }

        void ColorItem::set_rgb24(uint32_t value)
        {
            sAlias.reset();

            nRGB24 = value;
            enClass = CC_RGB24;
        }

        void ColorItem::set_rgb48(uint64_t value)
        {
            sAlias.reset();

            nRGB48 = value;
            enClass = CC_RGB24;
        }

        void ColorItem::set_none()
        {
            sAlias.reset();
            enClass = CC_NONE;
        }

        void ColorItem::swap(ColorItem & src)
        {
            sID.swap(src.sID);
            sAlias.swap(src.sAlias);
            lsp::swap(enClass, src.enClass);
            lsp::swap(nRGB48, src.nRGB48);
        }

        void ColorItem::swap(ColorItem * src)
        {
            sID.swap(src->sID);
            sAlias.swap(src->sAlias);
            lsp::swap(enClass, src->enClass);
            lsp::swap(nRGB48, src->nRGB48);
        }

    } /* namespace xpm */
} /* namespace lsp */


