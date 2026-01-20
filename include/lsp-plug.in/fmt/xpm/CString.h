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

#ifndef LSP_PLUG_IN_FMT_XPM_CSTRING_H_
#define LSP_PLUG_IN_FMT_XPM_CSTRING_H_

#include <lsp-plug.in/runtime/version.h>

#include <lsp-plug.in/lltl/shbuffer.h>

namespace lsp
{
    namespace xpm
    {
        typedef lltl::shbuffer<char> CStringBuffer;

        class LSP_RUNTIME_LIB_PUBLIC CString
        {
            private:
                CStringBuffer   hBuffer;
                char           *sValue;

            public:
                CString();
                CString(const char * src);
                CString(const CStringBuffer & src, size_t index);
                CString(const CString & src);
                CString(CString && src);
                CString & operator = (const CString & src);
                CString & operator = (CString && src);
                CString & operator = (const char * src);

                ~CString();

            public:
                inline const char *get() const          { return sValue; }
                inline const char * operator *() const  { return sValue; }
                inline bool is_null()                   { return sValue == NULL; }

                bool set(const char *value);
                bool set(const CStringBuffer & src, size_t index);
                bool set(const CString & src);
                void reset();

                void swap(CString * src);
                void swap(CString & src);
        };

    } /* namespace xpm */
} /* namespace lsp */


#endif /* LSP_PLUG_IN_FMT_XPM_CSTRING_H_ */
