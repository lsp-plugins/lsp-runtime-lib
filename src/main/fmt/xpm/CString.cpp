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

#include <lsp-plug.in/fmt/xpm/CString.h>
#include <lsp-plug.in/stdlib/string.h>

namespace lsp
{
    namespace xpm
    {
        CString::CString()
        {
            hBuffer     = NULL;
            sValue      = NULL;
        }

        CString::CString(const char *src)
        {
            hBuffer     = NULL;
            sValue      = strdup(src);
        }

        CString::CString(const CStringBuffer & src, size_t index)
        {
            hBuffer     = src;
            sValue      = const_cast<char *>(src.get(index));
        }

        CString::CString(const CString & src)
        {
            hBuffer     = src.hBuffer;
            sValue      = (hBuffer) ? src.sValue : strdup(src.sValue);
        }

        CString::CString(CString && src)
        {
            hBuffer     = lsp::move(src.hBuffer);
            sValue      = src.sValue;
            src.sValue  = NULL;
        }

        CString::~CString()
        {
            if ((!hBuffer) && (sValue))
            {
                free(sValue);
                sValue      = NULL;
            }
        }

        CString & CString::operator = (const CString & src)
        {
            if ((!hBuffer) && (sValue))
                free(sValue);

            hBuffer     = src.hBuffer;
            sValue      = (hBuffer) ? src.sValue : strdup(src.sValue);

            return *this;
        }

        CString & CString::operator = (CString && src)
        {
            if ((!hBuffer) && (sValue))
                free(sValue);

            hBuffer     = src.hBuffer;
            sValue      = (hBuffer) ? src.sValue : strdup(src.sValue);

            return *this;
        }

        CString & CString::operator = (const char * src)
        {
            if ((!hBuffer) && (sValue))
                free(sValue);

            hBuffer.reset();
            sValue      = strdup(src);

            return *this;
        }

        bool CString::set(const char *value)
        {
            char *tmp   = strdup(value);
            if (tmp == NULL)
                return false;

            if ((!hBuffer) && (sValue))
                free(sValue);

            hBuffer.reset();
            sValue      = tmp;

            return true;
        }

        bool CString::set(const CStringBuffer & src, size_t index)
        {
            hBuffer     = src;
            sValue      = const_cast<char *>(src.get(index));
            return true;
        }

        bool CString::set(const CString & src)
        {
            if (!src.hBuffer)
                return set(src.sValue);

            if ((!hBuffer) && (sValue))
                free(sValue);

            hBuffer     = src.hBuffer;
            sValue      = src.sValue;

            return true;
        }

        void CString::reset()
        {
            if (!hBuffer)
            {
                if (sValue)
                    free(sValue);
            }
            else
                hBuffer.reset();

            sValue = NULL;
        }

        void CString::swap(CString * src)
        {
            hBuffer.swap(src->hBuffer);
            lsp::swap(sValue, src->sValue);
        }

        void CString::swap(CString & src)
        {
            hBuffer.swap(src.hBuffer);
            lsp::swap(sValue, src.sValue);
        }

    } /* namespace xpm */
} /* namespace lsp */
