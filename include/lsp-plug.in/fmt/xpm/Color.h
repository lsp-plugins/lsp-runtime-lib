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

#ifndef LSP_PLUG_IN_FMT_XPM_COLOR_H_
#define LSP_PLUG_IN_FMT_XPM_COLOR_H_

#include <lsp-plug.in/runtime/version.h>

#include <lsp-plug.in/fmt/xpm/CString.h>
#include <lsp-plug.in/fmt/xpm/ColorItem.h>

namespace lsp
{
    namespace xpm
    {
        /**
         * XPM2/XPM3 color
         */
        class LSP_RUNTIME_LIB_PUBLIC Color
        {
            private:
                CString     sCode;
                ColorItem   sMono;          // XPM: mono visual ('m')
                ColorItem   sSymbolic;      // XPM: symbolic name ('s')
                ColorItem   sGray4;         // XPM: 4-level grayscale ('g4')
                ColorItem   sGray;          // XPM: grayscale with more than 4 levels ('g')
                ColorItem   sColor;         // XPM: color visual

            public:
                Color();
                explicit Color(const char * code);
                Color(const Color & src);
                Color(Color && src);

            public:
                inline const char *code() const                 { return sCode.get();       }
                inline bool set_code(const char *code)          { return sCode.set(code);   }
                inline bool set_code(const CStringBuffer & buf, size_t offset)    { return sCode.set(buf, offset);    }

                inline ColorItem & mono_visual()                { return sMono;             }
                inline const ColorItem & mono_visual() const    { return sMono;             }

                inline ColorItem & symbolic_visual()            { return sSymbolic;         }
                inline const ColorItem & symbolic_visual() const{ return sSymbolic;         }

                inline ColorItem & gray4_visual()               { return sGray4;            }
                inline const ColorItem & gray4_visual() const   { return sGray4;            }

                inline ColorItem & gray_visual()                { return sGray;             }
                inline const ColorItem & gray_visual() const    { return sGray;             }

                inline ColorItem & color_visual()               { return sColor;            }
                inline const ColorItem & color_visual() const   { return sColor;            }

            public:
                void swap(Color & src);
                void swap(Color * src);
        };

    } /* namespace xpm */
} /* namespace lsp */


#endif /* LSP_PLUG_IN_FMT_XPM_COLOR_H_ */
