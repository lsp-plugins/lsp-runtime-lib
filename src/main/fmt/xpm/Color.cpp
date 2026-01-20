/*
 * Copyright (C) 2026 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2026 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-runtime-lib
 * Created on: 20 янв. 2026 г.
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

#include <lsp-plug.in/fmt/xpm/Color.h>

namespace lsp
{
    namespace xpm
    {
        Color::Color()
        {
        }

        Color::Color(const char * code):
            sCode(code)
        {
        }

        Color::Color(const Color & src):
            sCode(src.sCode),
            sMono(src.sMono),
            sSymbolic(src.sSymbolic),
            sGray4(src.sGray4),
            sGray(src.sGray),
            sColor(src.sColor)
        {
        }

        Color::Color(Color && src):
            sCode(lsp::move(src.sCode)),
            sMono(lsp::move(src.sMono)),
            sSymbolic(lsp::move(src.sSymbolic)),
            sGray4(lsp::move(src.sGray4)),
            sGray(lsp::move(src.sGray)),
            sColor(lsp::move(src.sColor))
        {
        }

        void Color::swap(Color & src)
        {
            sCode.swap(src.sCode);
            sMono.swap(src.sMono);
            sSymbolic.swap(src.sSymbolic);
            sGray4.swap(src.sGray4);
            sGray.swap(src.sGray);
            sColor.swap(src.sColor);
        }

        void Color::swap(Color * src)
        {
            sCode.swap(src->sCode);
            sMono.swap(src->sMono);
            sSymbolic.swap(src->sSymbolic);
            sGray4.swap(src->sGray4);
            sGray.swap(src->sGray);
            sColor.swap(src->sColor);
        }

    } /* namespace xpm */
} /* namespace lsp */


