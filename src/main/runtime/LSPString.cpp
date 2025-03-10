/*
 * Copyright (C) 2024 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2024 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-runtime-lib
 * Created on: 30 авг. 2017 г.
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
#include <lsp-plug.in/common/status.h>
#include <lsp-plug.in/common/alloc.h>
#include <lsp-plug.in/stdlib/stdio.h>
#include <lsp-plug.in/stdlib/string.h>
#include <lsp-plug.in/io/charset.h>
#include <lsp-plug.in/runtime/LSPString.h>

#include <stdarg.h>
#include <stdlib.h>
#include <errno.h>
#include <wctype.h>

#define GRANULARITY     0x20
#define BUF_SIZE        0x200
//#define BUF_SIZE        16

#define XSAFE_TRANS(index, length, result) \
    if (index < 0) \
    { \
        if ((index += (length)) < 0) \
            return result; \
    } \
    else if (size_t(index) > (length)) \
        return result;

#define XSAFE_ITRANS(index, length, result) \
    if (index < 0) \
    { \
        if ((index += (length)) < 0) \
            return result; \
    } \
    else if (size_t(index) >= (length)) \
        return result;

namespace lsp
{
    static bool is_space(lsp_wchar_t c)
    {
        switch (c)
        {
            case ' ':
            case '\t':
            case '\n':
            case '\r':
            case '\v':
                return true;
            default:
                return false;
        }
    }

    LSPString::LSPString()
    {
        nLength     = 0;
        nCapacity   = 0;
        pData       = NULL;
        nHash       = 0;
        pTemp       = NULL;
    }

    LSPString::~LSPString()
    {
        truncate();
    }

#ifndef ARCH_LE
    int LSPString::xcmp(const lsp_wchar_t *a, const lsp_wchar_t *b, size_t n)
    {
        while (n--)
        {
            int32_t retval = int32_t(*(a++)) - int32_t(*(b++));
            if (retval != 0)
                return (retval > 0) ? 1 : -1;
        }
        return 0;
    }
#endif /* ARCH_LE */

    inline size_t LSPString::xlen(const lsp_wchar_t *s)
    {
        size_t i=0;
        while (s[i] != 0)
            ++i;
        return i;
    }

    inline size_t LSPString::u16len(const lsp_utf16_t *s)
    {
        size_t i=0;
        while (s[i] != 0)
            ++i;
        return i;
    }

    int LSPString::xcasecmp(const lsp_wchar_t *a, const lsp_wchar_t *b, size_t n)
    {
        while (n--)
        {
            int32_t retval = int32_t(lsp::to_lower(*(a++))) - int32_t(lsp::to_lower(*(b++)));
            if (retval != 0)
                return (retval > 0) ? 1 : -1;
        }
        return 0;
    }

    void LSPString::acopy(lsp_wchar_t *dst, const char *src, size_t n)
    {
        while (n--)
            *(dst++) = uint8_t(*(src++));
    }

    void LSPString::drop_temp()
    {
        if (pTemp == NULL)
            return;

        if (pTemp->pData != NULL)
            ::free(pTemp->pData);

        ::free(pTemp);
        pTemp = NULL;
    }

    void LSPString::clear()
    {
        drop_temp();
        nLength     = 0;
        nHash       = 0;
    }

    void LSPString::truncate()
    {
        drop_temp();

        nLength     = 0;
        nHash       = 0;
        nCapacity   = 0;
        if (pData == NULL)
            return;

        xfree(pData);
        pData       = NULL;
    }

    bool LSPString::truncate(size_t size)
    {
        drop_temp();
        if (size > nCapacity)
            return true;
        if (nLength > size)
        {
            nHash       = 0;
            nLength     = size;
        }

        lsp_wchar_t *v = xrealloc(pData, size);
        if ((v == NULL) && (size > 0))
            return false;

        pData       = (size > 0) ? v : NULL;
        nCapacity   = size;
        return true;
    }

    size_t LSPString::set_length(size_t length)
    {
        if (nLength <= length)
            return length;

        drop_temp();
        nHash       = 0;
        return nLength = length;
    }

    size_t LSPString::range_length(ssize_t first, ssize_t last) const
    {
        XSAFE_TRANS(first, nLength, 0);
        XSAFE_TRANS(last, nLength, 0);
        return (first < last) ? last - first : 0;
    }

    bool LSPString::size_reserve(size_t size)
    {
        if (size > 0)
        {
            lsp_wchar_t *v = xrealloc(pData, size);
            if (v == NULL)
                return false;
            pData   = v;
        }
        else if (pData != NULL)
        {
            ::free(pData);
            pData   = NULL;
        }

        nCapacity   = size;
        return true;
    }

    inline bool LSPString::cap_reserve(size_t size)
    {
        size_t ncap = (size + (GRANULARITY-1)) & (~(GRANULARITY-1));
        return (ncap > nCapacity) ? size_reserve(ncap) : true;
    }

    inline bool LSPString::cap_grow(size_t delta)
    {
        size_t avail = nCapacity - nLength;
        if (delta <= avail)
            return true;
        avail = nCapacity >> 1;
        if (avail < delta)
            avail = delta;
        return size_reserve(nCapacity + ((avail + (GRANULARITY-1)) & (~(GRANULARITY-1))));
    }

    void LSPString::reduce()
    {
        drop_temp();
        if (nCapacity <= nLength)
            return;
        lsp_wchar_t *v = xrealloc(pData, nLength);
        if ((v == NULL) && (nLength > 0))
            return;
        pData       = (nLength > 0) ? v : NULL;
        nCapacity   = nLength;
    }

    void LSPString::trim()
    {
        if ((pData == NULL) || (nLength <= 0))
            return;

        // Cut tail first
        lsp_wchar_t *p = &pData[nLength];
        while (nLength > 0)
        {
            if (!is_space(*(--p)))
                break;
            nLength--;
            nHash       = 0;
        }
        if (nLength <= 0)
            return;

        // Cut head
        p = pData;
        while (true)
        {
            if (!is_space(*p))
                break;
            p++;
        }
        if (p > pData)
        {
            nHash       = 0;
            nLength    -= (p - pData);
        }
        if (nLength <= 0)
            return;

        xmove(pData, p, nLength);
    }

    void LSPString::swap(LSPString *src)
    {
        if (src == this)
            return;

        size_t len      = src->nLength;
        size_t cap      = src->nCapacity;
        lsp_wchar_t *c  = src->pData;
        size_t hash     = src->nHash;

        src->nLength    = nLength;
        src->nCapacity  = nCapacity;
        src->pData      = pData;
        src->nHash      = nHash;

        nLength         = len;
        nCapacity       = cap;
        pData           = c;
        nHash           = hash;
    }

    bool LSPString::swap(ssize_t idx1, ssize_t idx2)
    {
        XSAFE_ITRANS(idx1, nLength, false);
        XSAFE_ITRANS(idx2, nLength, false);
        if (idx1 == idx2)
            return true;

        // Swap characters
        nHash           = 0;
        lsp_wchar_t c   = pData[idx1];
        pData[idx1]     = pData[idx2];
        pData[idx2]     = c;

        return true;
    }

    void LSPString::take(LSPString *src)
    {
        drop_temp();
        if (pData != NULL)
            xfree(pData);

        nLength         = src->nLength;
        nCapacity       = src->nCapacity;
        pData           = src->pData;
        nHash           = src->nHash;

        src->nLength    = 0;
        src->nCapacity  = 0;
        src->pData      = NULL;
        src->nHash      = 0;
    }

    void LSPString::take(LSPString &src)
    {
        drop_temp();
        if (pData != NULL)
            xfree(pData);

        nLength         = src.nLength;
        nCapacity       = src.nCapacity;
        pData           = src.pData;
        nHash           = src.nHash;

        src.nLength     = 0;
        src.nCapacity   = 0;
        src.pData       = NULL;
        src.nHash       = 0;
    }

    LSPString *LSPString::copy() const
    {
        LSPString *s = new LSPString();
        if (s == NULL)
            return s;

        s->nLength      = nLength;
        s->nCapacity    = nLength;
        if (s->nLength > 0)
        {
            s->pData        = xmalloc(nLength);
            if (s->pData == NULL)
            {
                delete s;
                return NULL;
            }

            xmove(s->pData, pData, nLength);
        }
        else
            s->pData        = NULL;

        return s;
    }

    LSPString *LSPString::release()
    {
        LSPString *str = new LSPString();
        if (str != NULL)
            str->swap(this);
        return str;
    }

    LSPString *LSPString::copy(ssize_t first) const
    {
        LSPString *s = new LSPString();
        if (s != NULL)
        {
            if (!s->set(this, first))
            {
                delete s;
                s = NULL;
            }
        }

        return s;
    }

    LSPString *LSPString::copy(ssize_t first, ssize_t last) const
    {
        LSPString *s = new LSPString();
        if (s != NULL)
        {
            if (!s->set(this, first, last))
            {
                delete s;
                s = NULL;
            }
        }

        return s;
    }

    lsp_wchar_t LSPString::at(ssize_t index) const
    {
        if (index < 0)
        {
            if ((index += nLength) < 0)
                return 0;
        }
        else if (size_t(index) >= nLength)
            return 0;

        return pData[index];
    }

    lsp_wchar_t LSPString::first() const
    {
        return (nLength > 0) ? pData[0] : 0;
    }

    lsp_wchar_t LSPString::last() const
    {
        return (nLength > 0) ? pData[nLength-1] : 0;
    }

    bool LSPString::set(lsp_wchar_t ch)
    {
        drop_temp();

        if (nCapacity == 0)
        {
            lsp_wchar_t *v = xmalloc(GRANULARITY);
            if (v == NULL)
                return false;
            v[0]        = ch;
            pData       = v;
            nCapacity   = GRANULARITY;
        }
        else
            pData[0]    = ch;

        nHash       = 0;
        nLength     = 1;
        return true;
    }

    bool LSPString::set(const lsp_wchar_t *arr)
    {
        return set(arr, xlen(arr));
    }

    bool LSPString::set(const lsp_wchar_t *arr, size_t n)
    {
        drop_temp();

        if (!cap_reserve(n))
            return false;

        xmove(pData, arr, n);
        nHash       = 0;
        nLength     = n;
        return true;
    }

    bool LSPString::set(ssize_t first, lsp_wchar_t ch)
    {
        XSAFE_ITRANS(first, nLength, false);
        pData[first]    = ch;
        nHash           = 0;
        return true;
    }

    bool LSPString::set(const LSPString *src)
    {
        if (src == this)
            return true;
        drop_temp();

        if (!cap_reserve(src->nLength))
            return false;
        if (src->nLength > 0)
            xmove(pData, src->pData, src->nLength);
        nLength     = src->nLength;
        nHash       = 0;
        return true;
    }

    bool LSPString::set(const LSPString *src, ssize_t first)
    {
        drop_temp();

        XSAFE_TRANS(first, src->nLength, false);
        ssize_t length = src->nLength - first;

        if (length > 0)
        {
            if (!cap_reserve(length))
                return false;
            xmove(pData, &src->pData[first], length);
            nLength     = length;
        }
        else
            nLength     = 0;

        nHash       = 0;
        return true;
    }

    bool LSPString::set(const LSPString *src, ssize_t first, ssize_t last)
    {
        drop_temp();

        XSAFE_TRANS(first, src->nLength, false);
        XSAFE_TRANS(last, src->nLength, false);

        ssize_t length = last - first;
        if (length > 0)
        {
            if (!cap_reserve(length))
                return false;
            xmove(pData, &src->pData[first], length);
            nLength     = length;
        }
        else
            nLength     = 0;

        nHash       = 0;
        return true;
    }

    bool LSPString::insert(ssize_t pos, lsp_wchar_t ch)
    {
        XSAFE_TRANS(pos, nLength, false);

        if (!cap_grow(1))
            return false;

        ssize_t length = nLength - pos;
        if (length > 0)
            xmove(&pData[pos+1], &pData[pos], length);

        pData[pos] = ch;
        nLength++;
        nHash       = 0;
        return true;
    }

    bool LSPString::insert(ssize_t pos, const lsp_wchar_t *arr, ssize_t n)
    {
        XSAFE_TRANS(pos, nLength, false);
        if (!cap_grow(n))
            return false;

        ssize_t count = nLength - pos;
        if (count > 0)
            xmove(&pData[pos+n], &pData[pos], count);
        xmove(&pData[pos], arr, n);
        nLength    += n;
        nHash       = 0;
        return true;
    }

    bool LSPString::insert(ssize_t pos, const LSPString *src)
    {
        if (src->nLength <= 0)
            return true;

        XSAFE_TRANS(pos, nLength, false);
        if (!cap_grow(src->nLength))
            return false;

        ssize_t count = nLength - pos;
        if (count > 0)
            xmove(&pData[pos+src->nLength], &pData[pos], count);
        xmove(&pData[pos], src->pData, src->nLength);
        nLength    += src->nLength;
        nHash       = 0;
        return true;
    }

    bool LSPString::insert(ssize_t pos, const LSPString *src, ssize_t first)
    {
        XSAFE_TRANS(first, src->nLength, false);
        ssize_t length = src->nLength - first;
        if (length <= 0)
            return true;

        XSAFE_TRANS(pos, nLength, false);
        if (!cap_grow(length))
            return false;

        ssize_t count = nLength - pos;
        if (count > 0)
            xmove(&pData[pos+length], &pData[pos], count);
        xmove(&pData[pos], &src->pData[first], length);
        nLength    += length;
        nHash       = 0;
        return true;
    }

    bool LSPString::insert(ssize_t pos, const LSPString *src, ssize_t first, ssize_t last)
    {
        XSAFE_TRANS(first, src->nLength, false);
        XSAFE_TRANS(last, src->nLength, false);
        ssize_t length = last - first;
        if (length <= 0)
            return true;

        XSAFE_TRANS(pos, nLength, false);
        if (!cap_grow(length))
            return false;

        ssize_t count = nLength - pos;
        if (count > 0)
            xmove(&pData[pos+length], &pData[pos], count);
        xmove(&pData[pos], &src->pData[first], length);
        nLength    += length;
        nHash       = 0;
        return true;
    }

    bool LSPString::append(char ch)
    {
        if (!cap_grow(1))
            return false;
        pData[nLength++] = uint8_t(ch);
        nHash            = 0;
        return true;
    }

    bool LSPString::append(lsp_wchar_t ch)
    {
        if (!cap_grow(1))
            return false;
        pData[nLength++] = ch;
        nHash            = 0;
        return true;
    }

    bool LSPString::append(lsp_swchar_t ch)
    {
        if (!cap_grow(1))
            return false;
        pData[nLength++] = ch;
        nHash            = 0;
        return true;
    }

    bool  LSPString::append(const lsp_wchar_t *arr)
    {
        return append(arr, xlen(arr));
    }

    bool LSPString::append(const lsp_wchar_t *arr, size_t n)
    {
        if (!cap_grow(n))
            return false;
        xmove(&pData[nLength], arr, n);
        nLength    += n;
        nHash       = 0;
        return true;
    }

    bool LSPString::append_ascii(const char *arr, size_t n)
    {
        if (!cap_grow(n))
            return false;
        acopy(&pData[nLength], arr, n);
        nLength    += n;
        nHash       = 0;
        return true;
    }

    bool LSPString::append_utf8(const char *arr, size_t n)
    {
        if (nLength <= 0)
            return set_utf8(arr, n);

        LSPString tmp;
        if (!tmp.set_utf8(arr, n))
            return false;
        return append(&tmp);
    }

    bool LSPString::append_utf16(const lsp_utf16_t *arr, size_t n)
    {
        if (nLength <= 0)
            return set_utf16(arr, n);

        LSPString tmp;
        if (!tmp.set_utf16(arr, n))
            return false;
        return append(&tmp);
    }

    bool LSPString::append_utf16(const lsp_utf16_t *arr)
    {
        return append_utf16(arr, u16len(arr));
    }

    bool LSPString::append(const LSPString *src)
    {
        if (src->nLength <= 0)
            return true;
        if (!cap_grow(src->nLength))
            return false;
        xmove(&pData[nLength], src->pData, src->nLength);
        nLength    += src->nLength;
        nHash       = 0;
        return true;
    }

    bool LSPString::append(const LSPString *src, ssize_t first)
    {
        XSAFE_TRANS(first, src->nLength, false);
        ssize_t length = src->nLength - first;
        if (length <= 0)
            return true;

        if (!cap_grow(length))
            return false;
        xmove(&pData[nLength], &src->pData[first], length);
        nLength    += length;
        nHash       = 0;
        return true;
    }

    bool LSPString::append(const LSPString *src, ssize_t first, ssize_t last)
    {
        XSAFE_TRANS(first, src->nLength, false);
        XSAFE_TRANS(last, src->nLength, false);
        ssize_t length = last - first;
        if (length <= 0)
            return true;

        if (!cap_grow(length))
            return false;
        xmove(&pData[nLength], &src->pData[first], length);
        nLength    += length;
        nHash       = 0;
        return true;
    }

    bool LSPString::prepend(lsp_wchar_t ch)
    {
        if (!cap_grow(1))
            return false;
        if (nLength > 0)
            xmove(&pData[1], pData, nLength);
        pData[0]    = ch;
        nLength     ++;
        nHash       = 0;
        return true;
    }

    bool LSPString::prepend(const lsp_wchar_t *arr, size_t n)
    {
        if (n <= 0)
            return true;
        if (!cap_grow(n))
            return false;

        if (nLength > 0)
            xmove(&pData[n], pData, nLength);
        xmove(pData, arr, n);
        nLength     += n;
        nHash       = 0;
        return true;
    }

    bool LSPString::prepend_ascii(const char *arr, size_t n)
    {
        if (n <= 0)
            return true;
        if (!cap_grow(n))
            return false;

        if (nLength > 0)
            xmove(&pData[n], pData, nLength);
        acopy(pData, arr, n);
        nLength    += n;
        nHash       = 0;
        return true;
    }

    bool LSPString::prepend_utf8(const char *arr, size_t n)
    {
        if (nLength <= 0)
            return set_utf8(arr, n);

        LSPString tmp;
        if (!tmp.set_utf8(arr, n))
            return false;
        return prepend(&tmp);
    }

    bool LSPString::prepend(const LSPString *src)
    {
        if (src->nLength <= 0)
            return true;
        if (!cap_grow(src->nLength))
            return false;

        if (nLength > 0)
            xmove(&pData[src->nLength], pData, nLength);
        xmove(pData, src->pData, src->nLength);
        nLength    += src->nLength;
        nHash       = 0;
        return true;
    }

    bool LSPString::prepend(const LSPString *src, ssize_t first)
    {
        XSAFE_TRANS(first, src->nLength, false);
        ssize_t length = src->nLength - first;
        if (length <= 0)
            return true;

        if (!cap_grow(length))
            return false;
        if (nLength > 0)
            xmove(&pData[length], pData, nLength);
        xmove(pData, &src->pData[first], length);
        nLength    += length;
        nHash       = 0;
        return true;
    }

    bool LSPString::prepend(const LSPString *src, ssize_t first, ssize_t last)
    {
        XSAFE_TRANS(first, src->nLength, false);
        XSAFE_TRANS(last, src->nLength, false);
        ssize_t length = last - first;
        if (length <= 0)
            return true;

        if (!cap_grow(length))
            return false;
        if (nLength > 0)
            xmove(&pData[length], pData, nLength);
        xmove(pData, &src->pData[first], length);
        nLength    += length;
        nHash       = 0;
        return true;
    }

    bool LSPString::ends_with(lsp_wchar_t ch) const
    {
        return (nLength > 0) ? pData[nLength-1] == ch : false;
    }

    bool LSPString::ends_with_nocase(lsp_wchar_t ch) const
    {
        if (nLength <= 0)
            return false;
        return lsp::to_lower(pData[nLength-1]) == lsp::to_lower(ch);
    }

    bool LSPString::ends_with(const LSPString *src) const
    {
        if (src->nLength <= 0)
            return true;

        ssize_t offset = nLength - src->nLength;
        if (offset < 0)
            return false;

        return xcmp(&pData[offset], src->pData, src->nLength) == 0;
    }

    bool LSPString::ends_with_ascii(const char *src) const
    {
        size_t len = strlen(src);
        return (len <= nLength) ? starts_with_ascii(src, nLength - len) : false;
    }

    bool LSPString::ends_with_ascii_nocase(const char *src) const
    {
        size_t len = strlen(src);
        return (len <= nLength) ? starts_with_ascii_nocase(src, nLength - len) : false;
    }

    bool LSPString::ends_with_nocase(const LSPString *src) const
    {
        if (src->nLength <= 0)
            return true;

        ssize_t offset = nLength - src->nLength;
        if (offset < 0)
            return false;

        return xcasecmp(&pData[offset], src->pData, src->nLength) == 0;
    }

    bool LSPString::starts_with(lsp_wchar_t ch, size_t offset) const
    {
        return (offset < nLength) ? pData[offset] == ch : false;
    }

    bool LSPString::starts_with_nocase(lsp_wchar_t ch, size_t offset) const
    {
        if (offset > nLength)
            return false;
        return lsp::to_lower(pData[offset]) == lsp::to_lower(ch);
    }

    bool LSPString::starts_with(const LSPString *src, size_t offset) const
    {
        if (src->nLength <= 0)
            return true;

        if (nLength < (src->nLength + offset))
            return false;

        return xcmp(&pData[offset], src->pData, src->nLength) == 0;
    }

    bool LSPString::starts_with_ascii(const char *str, size_t offset) const
    {
        if (offset > nLength)
            return false;

        for (size_t i=offset, n=nLength; (i < n); ++i)
        {
            lsp_wchar_t c = uint8_t(*(str++));
            if (c == 0)
                return true;
            else if (c != pData[i])
                return false;
        }
        return (*str == '\0');
    }

    bool LSPString::starts_with_nocase(const LSPString *src, size_t offset) const
    {
        if (src->nLength <= 0)
            return true;

        if (nLength < (offset + src->nLength))
            return false;

        return xcasecmp(&pData[offset], src->pData, src->nLength) == 0;
    }

    bool LSPString::starts_with_ascii_nocase(const char *str, size_t offset) const
    {
        if (offset > nLength)
            return false;

        for (size_t i=offset, n=nLength; (i < n); ++i)
        {
            lsp_wchar_t c = uint8_t(*(str++));
            if (c == 0)
                return true;
            else if (lsp::to_lower(c) != lsp::to_lower(pData[i]))
                return false;
        }
        return (*str == '\0');
    }

    bool LSPString::remove()
    {
        drop_temp();
        nLength     = 0;
        nHash       = 0;
        return true;
    }

    bool LSPString::remove(ssize_t first)
    {
        XSAFE_TRANS(first, nLength, false);
        nLength     = first;
        nHash       = 0;
        return true;
    }

    bool LSPString::remove(ssize_t first, ssize_t last)
    {
        XSAFE_TRANS(first, nLength, false);
        XSAFE_TRANS(last, nLength, false);
        ssize_t length = last - first;
        if (length <= 0)
            return true;

        ssize_t count = nLength - last;
        if (count > 0)
            xmove(&pData[first], &pData[last], count);

        nLength    -= length;
        nHash       = 0;
        return true;
    }

    bool LSPString::remove_last()
    {
        if (nLength <= 0)
            return false;
        --nLength;
        nHash       = 0;
        return true;
    }

    void LSPString::reverse()
    {
        drop_temp();
        nHash       = 0;

        size_t n = (nLength >> 1);
        lsp_wchar_t *h = pData, *t = &pData[nLength];
        while (n--)
        {
            lsp_wchar_t c = *h;
            *(h++)  = *(--t);
            *t      = c;
        }
    }

    void LSPString::shuffle()
    {
        if (nLength < 2)
            return;

        nHash       = 0;
        size_t n    = nLength * 2;
        size_t idx1 = rand() % nLength, idx2;

        while (n--)
        {
            // Generate random indexes
            idx1 = (idx1 + rand()) % nLength;
            idx2 = (idx1 + rand()) % nLength;
            if (idx1 == idx2)
                continue;

            // Swap characters
            lsp_wchar_t c   = pData[idx1];
            pData[idx1]     = pData[idx2];
            pData[idx2]     = c;
        }
    }

    bool LSPString::replace(ssize_t pos, lsp_wchar_t ch)
    {
        XSAFE_TRANS(pos, nLength, false);

        if (size_t(pos) < nLength)
        {
            pData[pos]  = ch;
            nLength     = pos;
            nHash       = 0;

            return true;
        }

        // Append at the tail
        return append(ch);
    }

    bool LSPString::replace(ssize_t pos, const lsp_wchar_t *arr, size_t n)
    {
        XSAFE_TRANS(pos, nLength, false);

        if (!cap_reserve(pos + n))
            return false;

        xmove(&pData[pos], arr, n);
        nLength     = pos + n;
        nHash       = 0;
        return true;
    }

    bool LSPString::replace(ssize_t pos, const LSPString *src)
    {
        XSAFE_TRANS(pos, nLength, false);

        if (!cap_reserve(pos + src->nLength))
            return false;

        xmove(&pData[pos], src->pData, src->nLength);
        nLength     = pos + src->nLength;
        nHash       = 0;
        return true;
    }

    bool LSPString::replace(ssize_t pos, const LSPString *src, ssize_t first)
    {
        XSAFE_TRANS(pos, nLength, false);
        XSAFE_TRANS(first, src->nLength, false);
        ssize_t length = src->nLength - first;

        if (length > 0)
        {
            if (!cap_reserve(pos + length))
                return false;

            xmove(&pData[pos], &src->pData[first], length);
        }
        nLength     = pos + length;
        nHash       = 0;
        return true;
    }

    bool LSPString::replace(ssize_t pos, const LSPString *src, ssize_t first, ssize_t last)
    {
        XSAFE_TRANS(pos, nLength, false);
        XSAFE_TRANS(first, src->nLength, false);
        XSAFE_TRANS(last, src->nLength, false);
        ssize_t length = lsp_max(last - first, 0);

        if (length > 0)
        {
            if (!cap_reserve(pos + length))
                return false;

            xmove(&pData[pos], &src->pData[first], length);
        }

        nLength     = pos + length;
        nHash       = 0;
        return true;
    }

    bool LSPString::replace(ssize_t first, ssize_t last, lsp_wchar_t ch)
    {
        XSAFE_TRANS(first, nLength, false);
        XSAFE_TRANS(last, nLength, false);
        ssize_t count = last - first;
        if (count < 0)
            count = 0;

        if (!cap_reserve(nLength - count + 1))
            return false;

        ssize_t tail = nLength - first - count;
        if (tail > 0)
            xmove(&pData[first + 1], &pData[tail], nLength - tail);
        pData[first]    = ch;

        nLength     = nLength - count + 1;
        nHash       = 0;
        return true;
    }

    bool LSPString::replace(ssize_t first, ssize_t last, const lsp_wchar_t *arr, size_t n)
    {
        XSAFE_TRANS(first, nLength, false);
        XSAFE_TRANS(last, nLength, false);
        ssize_t count = lsp_max(last - first, 0);

        if (!cap_reserve(nLength - count + n))
            return false;

        ssize_t tail = nLength - first - count;
        if (tail > 0)
            xmove(&pData[first + n], &pData[tail], nLength - tail);
        if (n > 0)
            xmove(&pData[first], arr, n);
        nLength     = nLength - count + n;
        nHash       = 0;
        return true;
    }

    bool LSPString::replace(ssize_t first, ssize_t last, const LSPString *src)
    {
        XSAFE_TRANS(first, nLength, false);
        XSAFE_TRANS(last, nLength, false);
        ssize_t count = lsp_max(last - first, 0);

        if (!cap_reserve(nLength - count + src->nLength))
            return false;

        ssize_t tail = nLength - first - count;
        if (tail > 0)
            xmove(&pData[first + src->nLength], &pData[tail], nLength - tail);
        if (src->nLength > 0)
            xmove(&pData[first], src->pData, src->nLength);
        nLength     = nLength - count + src->nLength;
        nHash       = 0;
        return true;
    }

    bool LSPString::replace(ssize_t first, ssize_t last, const LSPString *src, ssize_t sfirst)
    {
        XSAFE_TRANS(first, nLength, false);
        XSAFE_TRANS(last, nLength, false);
        ssize_t count = last - first;
        if (count < 0)
            count = 0;

        XSAFE_TRANS(sfirst, src->nLength, false);
        ssize_t scount = src->nLength - sfirst;

        if (!cap_reserve(nLength - count + scount))
            return false;

        ssize_t tail = nLength - first - count;
        if (tail > 0)
            xmove(&pData[first + scount], &pData[tail], nLength - tail);
        if (scount > 0)
            xmove(&pData[first], &src->pData[sfirst], scount);

        nLength     = nLength - count + scount;
        nHash       = 0;
        return true;
    }

    bool LSPString::replace(ssize_t first, ssize_t last, const LSPString *src, ssize_t sfirst, ssize_t slast)
    {
        XSAFE_TRANS(first, nLength, false);
        XSAFE_TRANS(last, nLength, false);
        ssize_t count = lsp_max(last - first, 0);

        XSAFE_TRANS(sfirst, src->nLength, false);
        XSAFE_TRANS(slast, src->nLength, false);
        ssize_t scount = lsp_max(slast - sfirst, 0);

        if (!cap_reserve(nLength - count + scount))
            return false;

        ssize_t tail = nLength - first - count;
        if (tail > 0)
            xmove(&pData[first + scount], &pData[tail], nLength - tail);
        if (scount > 0)
            xmove(&pData[first], &src->pData[sfirst], scount);

        nLength     = nLength - count + scount;
        nHash       = 0;
        return true;
    }

    size_t LSPString::replace_all(lsp_wchar_t ch, lsp_wchar_t rep)
    {
        size_t n = 0;
        for (size_t i=0; i<nLength; ++i)
        {
            if (pData[i] == ch)
            {
                pData[i] = rep;
                ++n;
            }
        }
        if (n > 0)
            nHash       = 0;

        return n;
    }

    ssize_t LSPString::index_of(ssize_t start, const LSPString *str) const
    {
        XSAFE_TRANS(start, nLength, -1);
        if (str->nLength <= 0)
            return start;

        ssize_t last = nLength - str->nLength;
        while (start <= last)
        {
            if (xcmp(&pData[start], str->pData, str->nLength) == 0)
                return start;
            start ++;
        }
        return -1;
    }

    ssize_t LSPString::index_of(const LSPString *str) const
    {
        if (str->nLength <= 0)
            return 0;

        ssize_t start = 0, last = nLength - str->nLength;
        while (start <= last)
        {
            if (xcmp(&pData[start], str->pData, str->nLength) == 0)
                return start;
            start ++;
        }
        return -1;
    }

    ssize_t LSPString::index_of(ssize_t start, lsp_wchar_t ch) const
    {
        XSAFE_TRANS(start, nLength, -1);

        ssize_t length = nLength;
        while (start < length)
        {
            if (pData[start] == ch)
                return start;
            start ++;
        }

        return -1;
    }

    ssize_t LSPString::index_of(lsp_wchar_t ch) const
    {
        for (size_t start = 0; start < nLength; ++start)
        {
            if (pData[start] == ch)
                return start;
        }
        return -1;
    }

    ssize_t LSPString::rindex_of(ssize_t start, const LSPString *str) const
    {
        if (start > ssize_t(nLength))
            return -1;

        start -= str->nLength;
        while (start >= 0)
        {
            if (xcmp(&pData[start], str->pData, str->nLength) == 0)
                return start;
            start --;
        }
        return -1;
    }

    ssize_t LSPString::rindex_of(const LSPString *str) const
    {
        if (str->nLength <= 0)
            return 0;

        ssize_t start = nLength - str->nLength;
        while (start >= 0)
        {
            if (xcmp(&pData[start], str->pData, str->nLength) == 0)
                return start;
            start --;
        }
        return -1;
    }

    ssize_t LSPString::rindex_of(ssize_t start, lsp_wchar_t ch) const
    {
        XSAFE_ITRANS(start, nLength, -1);

        while (start >= 0)
        {
            if (pData[start] == ch)
                return start;
            start --;
        }

        return -1;
    }

    ssize_t LSPString::rindex_of(lsp_wchar_t ch) const
    {
        for (ssize_t start=nLength-1; start >= 0; --start)
        {
            if (pData[start] == ch)
                return start;
        }
        return -1;
    }

    ssize_t LSPString::index_of_nocase(ssize_t start, const LSPString *str) const
    {
        XSAFE_TRANS(start, nLength, -1);
        if (str->nLength <= 0)
            return start;

        ssize_t last = nLength - str->nLength;
        while (start <= last)
        {
            if (xcasecmp(&pData[start], str->pData, str->nLength) == 0)
                return start;
            start ++;
        }
        return -1;
    }

    ssize_t LSPString::index_of_nocase(const LSPString *str) const
    {
        if (str->nLength <= 0)
            return 0;

        ssize_t start = 0, last = nLength - str->nLength;
        while (start <= last)
        {
            if (xcasecmp(&pData[start], str->pData, str->nLength) == 0)
                return start;
            start ++;
        }
        return -1;
    }

    ssize_t LSPString::index_of_nocase(ssize_t start, lsp_wchar_t ch) const
    {
        XSAFE_TRANS(start, nLength, -1);

        ssize_t length = nLength;
        ch = lsp::to_lower(ch);
        while (start < length)
        {
            if (lsp::to_lower(pData[start]) == ch)
                return start;
            start ++;
        }

        return -1;
    }

    ssize_t LSPString::index_of_nocase(lsp_wchar_t ch) const
    {
        ch = lsp::to_lower(ch);
        for (size_t start = 0; start < nLength; ++start)
        {
            if (lsp::to_lower(pData[start]) == ch)
                return start;
        }
        return -1;
    }

    ssize_t LSPString::rindex_of_nocase(ssize_t start, const LSPString *str) const
    {
        if (start > ssize_t(nLength))
            return -1;

        start -= str->nLength;
        while (start >= 0)
        {
            if (xcasecmp(&pData[start], str->pData, str->nLength) == 0)
                return start;
            start --;
        }
        return -1;
    }

    ssize_t LSPString::rindex_of_nocase(const LSPString *str) const
    {
        if (str->nLength <= 0)
            return 0;

        ssize_t start = nLength - str->nLength;
        while (start >= 0)
        {
            if (xcasecmp(&pData[start], str->pData, str->nLength) == 0)
                return start;
            start --;
        }
        return -1;
    }

    ssize_t LSPString::rindex_of_nocase(ssize_t start, lsp_wchar_t ch) const
    {
        XSAFE_ITRANS(start, nLength, -1);

        ch = lsp::to_lower(ch);
        while (start >= 0)
        {
            if (lsp::to_lower(pData[start]) == ch)
                return start;
            start --;
        }

        return -1;
    }

    ssize_t LSPString::rindex_of_nocase(lsp_wchar_t ch) const
    {
        ch = lsp::to_lower(ch);
        for (ssize_t start=nLength-1; start >= 0; --start)
        {
            if (lsp::to_lower(pData[start]) == ch)
                return start;
        }
        return -1;
    }

    LSPString *LSPString::substring(ssize_t first) const
    {
        XSAFE_TRANS(first, nLength, NULL);
        ssize_t length = nLength - first;

        LSPString *s = new LSPString();
        if (s == NULL)
            return s;

        s->nLength      = length;
        s->nCapacity    = length;

        if (length > 0)
        {
            s->pData        = xmalloc(length);
            if (s->pData == NULL)
            {
                delete s;
                return NULL;
            }

            xmove(s->pData, &pData[first], length);
        }
        else
            s->pData        = NULL;

        return s;
    }

    LSPString *LSPString::substring(ssize_t first, ssize_t last) const
    {
        XSAFE_TRANS(first, nLength, NULL);
        XSAFE_TRANS(last, nLength, NULL);
        ssize_t length  = last - first;
        if (length < 0)
            length          = 0;

        LSPString *s = new LSPString();
        if (s == NULL)
            return s;

        s->nLength      = length;
        s->nCapacity    = length;

        if (length > 0)
        {
            s->pData        = xmalloc(length);
            if (s->pData == NULL)
            {
                delete s;
                return NULL;
            }

            xmove(s->pData, &pData[first], length);
        }
        else
            s->pData        = NULL;

        return s;
    }

    int LSPString::compare_to(const lsp_wchar_t *src) const
    {
        return compare_to(src, xlen(src));
    }

    int LSPString::compare_to(const lsp_wchar_t *src, size_t len) const
    {
        ssize_t n = (nLength > len) ? len : nLength;
        const lsp_wchar_t *a = pData, *b = src;

        while (n--)
        {
            int retval = int(*(a++)) - int(*(b++));
            if (retval != 0)
                return retval;
        }

        if (a < &pData[nLength])
            return int(*a);
        else if (b < &src[len])
            return -int(*b);

        return 0;
    }

    int LSPString::compare_to_ascii(const char *src) const
    {
        size_t i=0;
        for ( ; i<nLength; ++i)
        {
            if (src[i] == '\0')
                return pData[i];
            int retval = int(pData[i]) - uint8_t(src[i]);
            if (retval != 0)
                return retval;
        }
        return -int(uint8_t(src[i]));
    }

    int LSPString::compare_to_utf8(const char *src) const
    {
        LSPString tmp;
        return (tmp.set_utf8(src)) ? compare_to(&tmp) : 0;
    }

    int LSPString::compare_to_utf16(const lsp_utf16_t *src) const
    {
        LSPString tmp;
        return (tmp.set_utf16(src)) ? compare_to(&tmp) : 0;
    }

    int LSPString::compare_to_ascii_nocase(const char *src) const
    {
        size_t i=0;
        for ( ; i<nLength; ++i)
        {
            if (src[i] == '\0')
                return pData[i];
            int retval = int(::lsp::to_lower(pData[i])) - ::lsp::to_lower(uint8_t(src[i]));
            if (retval != 0)
                return retval;
        }
        return -int(uint8_t(src[i]));
    }

    int LSPString::compare_to_nocase(const lsp_wchar_t *src, size_t len) const
    {
        ssize_t n = (nLength > len) ? len : nLength;
        const lsp_wchar_t *a = pData, *b = src;

        while (n--)
        {
            int retval = int(::lsp::to_lower(*(a++))) - int(::lsp::to_lower(*(b++)));
            if (retval != 0)
                return retval;
        }

        if (a < &pData[nLength])
            return int(*a);
        else if (b < &src[len])
            return -int(*b);

        return 0;
    }

    int LSPString::compare_to_nocase(const lsp_wchar_t *src) const
    {
        return compare_to_nocase(src, xlen(src));
    }

    int LSPString::compare_to_utf8_nocase(const char *src) const
    {
        LSPString tmp;
        return (tmp.set_utf8(src)) ? compare_to_nocase(&tmp) : 0;
    }

    int LSPString::compare_to_utf16_nocase(const lsp_utf16_t *src) const
    {
        LSPString tmp;
        return (tmp.set_utf16(src)) ? compare_to_nocase(&tmp) : 0;
    }

    size_t LSPString::tolower()
    {
        for (size_t i=0; i<nLength; ++i)
            pData[i] = lsp::to_lower(pData[i]);
        nHash       = 0;
        return nLength;
    }

    size_t LSPString::tolower(ssize_t first)
    {
        XSAFE_TRANS(first, nLength, 0);
        ssize_t n = nLength - first;
        if (n <= 0)
            return 0;

        lsp_wchar_t *ptr = &pData[first];
        for (ssize_t i=0; i<n; ++i)
            ptr[i] = lsp::to_lower(ptr[i]);

        nHash       = 0;
        return n;
    }

    size_t LSPString::tolower(ssize_t first, ssize_t last)
    {
        XSAFE_TRANS(first, nLength, 0);
        XSAFE_TRANS(last, nLength, 0);
        if (last < first)
        {
            ssize_t tmp = last;
            last = first;
            first = tmp;
        }

        ssize_t n = last - first;
        lsp_wchar_t *ptr = &pData[first];
        for (; first < last; ++first)
            ptr[first] = lsp::to_lower(ptr[first]);
        nHash       = 0;
        return n;
    }

    size_t LSPString::toupper()
    {
        for (size_t i=0; i<nLength; ++i)
            pData[i] = lsp::to_upper(pData[i]);
        nHash       = 0;
        return nLength;
    }

    size_t LSPString::toupper(ssize_t first)
    {
        XSAFE_TRANS(first, nLength, 0);
        ssize_t n = nLength - first;
        if (n <= 0)
            return 0;

        lsp_wchar_t *ptr = &pData[first];
        for (ssize_t i=0; i<n; ++i)
            ptr[i] = lsp::to_upper(ptr[i]);
        nHash       = 0;
        return n;
    }

    size_t LSPString::toupper(ssize_t first, ssize_t last)
    {
        XSAFE_TRANS(first, nLength, 0);
        XSAFE_TRANS(last, nLength, 0);
        if (last < first)
        {
            ssize_t tmp = last;
            last = first;
            first = tmp;
        }
        ssize_t n   = last - first;
        lsp_wchar_t *ptr = &pData[first];
        for (; first < last; ++first)
            ptr[first] = lsp::to_upper(ptr[first]);
        nHash       = 0;
        return n;
    }

    bool LSPString::equals(const lsp_wchar_t *src, size_t len) const
    {
        if (nLength != len)
            return false;
        if (nLength <= 0)
            return true;

        return xcmp(pData, src, nLength) == 0;
    }

    bool LSPString::equals(const lsp_wchar_t *src) const
    {
        return equals(src, xlen(src));
    }

    bool LSPString::equals(const LSPString *src, ssize_t first, ssize_t last) const
    {
        XSAFE_TRANS(first, src->nLength, false);
        XSAFE_TRANS(last, src->nLength, false);

        return equals(&src->pData[first], last - first);
    }

    bool LSPString::equals_nocase(const lsp_wchar_t *src, size_t len) const
    {
        if (nLength != len)
            return false;

        const lsp_wchar_t *a = pData, *b = src;
        for (size_t i=nLength; i>0; --i)
        {
            if (lsp::to_lower(*(a++)) != lsp::to_lower(*(b++)))
                return false;
        }

        return true;
    }

    bool LSPString::equals_nocase(const lsp_wchar_t *src) const
    {
        return equals_nocase(src, xlen(src));
    }

    bool LSPString::contains_at(ssize_t index, const lsp_wchar_t *src) const
    {
        const lsp_wchar_t *p = &pData[index];
        while (*src != '\0')
        {
            if (size_t(++index) > nLength)
                return false;
            if (*(p++) != *(src++))
                return false;
        }

        return true;
    }

    bool LSPString::contains_at(ssize_t index, const lsp_wchar_t *src, size_t len) const
    {
        if (nLength < (index + len))
            return false;

        const lsp_wchar_t *p = &pData[index];
        for (size_t i=0; i<len; ++i)
            if (p[i] != src[i])
                return false;

        return true;
    }

    bool LSPString::contains_at_ascii(ssize_t index, const char *src) const
    {
        const lsp_wchar_t *p = &pData[index];
        while (*src != '\0')
        {
            if (size_t(++index) > nLength)
                return false;
            if (lsp_wchar_t(*(p++)) != uint8_t(*(src++)))
                return false;
        }

        return true;
    }

    bool LSPString::contains_at_utf8(ssize_t index, const char *src) const
    {
        LSPString tmp;
        return (tmp.set_utf8(src)) ? contains_at(index, &tmp) : false;
    }

    bool LSPString::contains_at_utf16(ssize_t index, const lsp_utf16_t *src) const
    {
        LSPString tmp;
        return (tmp.set_utf16(src)) ? contains_at(index, &tmp) : false;
    }

    bool LSPString::set_utf8(const char *s, size_t n)
    {
        LSPString   tmp;
        lsp_wchar_t ch;

        while (lsp_utf32_t(ch = read_utf8_streaming(&s, &n, true)) != LSP_UTF32_EOF)
        {
            // Append code point
            if (!tmp.append(ch))
                return false;
        }
        if (n > 0)
            return false;

        tmp.swap(this);
        return true;
    }

    bool LSPString::set_utf16(const lsp_utf16_t *s)
    {
        return set_utf16(s, u16len(s));
    }

    bool LSPString::set_utf16(const lsp_utf16_t *s, size_t n)
    {
        LSPString   tmp;
        lsp_wchar_t ch;

        while (lsp_utf32_t(ch = read_utf16_streaming(&s, &n, true)) != LSP_UTF32_EOF)
        {
            // Append code point
            if (!tmp.append(ch))
                return false;
        }
        if (n > 0)
            return false;

        tmp.swap(this);
        return true;
    }

    bool LSPString::set_utf16le(const lsp_utf16_t *s)
    {
        return set_utf16le(s, u16len(s));
    }

    bool LSPString::set_utf16le(const lsp_utf16_t *s, size_t n)
    {
        LSPString   tmp;
        lsp_wchar_t ch;

        while (lsp_utf32_t(ch = read_utf16le_streaming(&s, &n, true)) != LSP_UTF32_EOF)
        {
            // Append code point
            if (!tmp.append(ch))
                return false;
        }
        if (n > 0)
            return false;

        tmp.swap(this);
        return true;
    }

    bool LSPString::set_utf16be(const lsp_utf16_t *s)
    {
        return set_utf16be(s, u16len(s));
    }

    bool LSPString::set_utf16be(const lsp_utf16_t *s, size_t n)
    {
        LSPString   tmp;
        lsp_wchar_t ch;

        while (lsp_utf32_t(ch = read_utf16be_streaming(&s, &n, true)) != LSP_UTF32_EOF)
        {
            // Append code point
            if (!tmp.append(ch))
                return false;
        }
        if (n > 0)
            return false;

        tmp.swap(this);
        return true;
    }

#if defined(PLATFORM_WINDOWS)
    bool LSPString::set_native(const char *s, size_t n, const char *charset)
    {
        if (s == NULL)
            return false;
        else if (n == 0)
        {
            nLength = 0;
            return true;
        }

        // Get codepage
        ssize_t cp = codepage_from_name(charset);
        if (cp < 0)
            return false;

        // Estimate size of string in memory
        ssize_t slen = multibyte_to_widechar(cp, const_cast<char *>(s), &n, NULL, NULL);
        if (slen <= 0)
            return false;

        // Perform native -> utf-16 encoding
        lsp_utf16_t *buf = reinterpret_cast<lsp_utf16_t *>(::malloc(slen * sizeof(lsp_utf16_t)));
        if (buf == NULL)
            return false;

        size_t bytes  = slen;
        slen    = multibyte_to_widechar(cp, const_cast<char *>(s), &n, buf, &bytes);
        if (slen <= 0)
        {
            free(buf);
            return false;
        }

        // Set encoded utf-16 values
        bool res = set_utf16(buf, slen);
        free(buf);

        return res;
    }
#else
    bool LSPString::set_native(const char *s, size_t n, const char *charset)
    {
        if (s == NULL)
            return false;
        else if (n == 0)
        {
            nLength = 0;
            return true;
        }

        char buf[BUF_SIZE];
        LSPString temp;

        // Open conversion
        iconv_t cd = init_iconv_to_wchar_t(charset);
        if (cd == iconv_t(-1))
            return set_utf8(s, n);

        size_t insize   = n;
        size_t outsize  = BUF_SIZE;
        char *inbuf     = const_cast<char *>(s);
        char *outbuf    = buf;

        while (insize > 0)
        {
            // Do the conversion
            size_t nconv = iconv(cd, &inbuf, &insize, &outbuf, &outsize);

            if (nconv == (size_t) -1)
            {
                switch (errno)
                {
                    case E2BIG:
                    case EINVAL:
                        break;
                    default:
                        iconv_close(cd);
                        return false;
                }
            }

            // Append set of converted characters to string
            ssize_t n_chars = (BUF_SIZE - outsize) / sizeof(lsp_wchar_t);
            if (n_chars > 0)
            {
                if (!temp.append(reinterpret_cast<lsp_wchar_t *>(&buf[0]), n_chars))
                {
                    iconv_close(cd);
                    return false;
                }
            }

            size_t right = n_chars * sizeof(lsp_wchar_t);
            ssize_t tail = (outsize - right) % sizeof(lsp_wchar_t);
            if (tail > 0)
            {
                // If there is a tail, copy it to the start of buffer
                ::memmove(buf, &buf[right], tail);
                outbuf  = &buf[tail];
                outsize = BUF_SIZE - tail;
            }
            else
            {
                // Otherwise just reset buffer's pointer
                outbuf  = buf;
                outsize = BUF_SIZE;
            }
        }

        // Close descriptor
        iconv_close(cd);
        take(&temp);
        return true;
    }
#endif /* PLATFORM_WINDOWS */

    bool LSPString::set_ascii(const char *s, size_t n)
    {
        LSPString   tmp;
        if (!tmp.reserve(n))
            return false;

        acopy(tmp.pData, s, n);
        take(&tmp);
        nLength     = n;
        nHash       = 0;
        return true;
    }

    const char *LSPString::get_utf8(ssize_t first, ssize_t last) const
    {
        XSAFE_TRANS(first, nLength, NULL);
        XSAFE_TRANS(last, nLength, NULL);
        if (first > last)
            return NULL;

        if (pTemp != NULL)
            pTemp->nOffset      = 0;

        char temp[BUF_SIZE + 16];
        char *th = temp, *tt = &temp[BUF_SIZE];

        for (ssize_t i=first; i<last; ++i)
        {
            lsp_wchar_t ch = pData[i];
            write_utf8_codepoint(&th, ch);

            if (th >= tt)
            {
                if (!append_temp(temp, th - temp))
                    return NULL;
                th  = temp;
            }
        }

        *(th++) = '\0';
        if (!append_temp(temp, th - temp))
            return NULL;

        return pTemp->pData;
    }

    const lsp_utf16_t *LSPString::get_utf16(ssize_t first, ssize_t last) const
    {
        XSAFE_TRANS(first, nLength, NULL);
        XSAFE_TRANS(last, nLength, NULL);
        if (first > last)
            return NULL;

        if (pTemp != NULL)
            pTemp->nOffset      = 0;

        lsp_utf16_t temp[BUF_SIZE + 8];
        lsp_utf16_t *th = temp, *tt = &temp[BUF_SIZE];

        for (ssize_t i=first; i<last; ++i)
        {
            lsp_wchar_t ch = pData[i];
            write_utf16_codepoint(&th, ch);

            if (th >= tt)
            {
                if (!append_temp(reinterpret_cast<char *>(temp), (th - temp) * sizeof(lsp_utf16_t)))
                    return NULL;
                th  = temp;
            }
        }

        *(th++) = '\0';
        if (!append_temp(reinterpret_cast<char *>(temp), (th - temp) * sizeof(lsp_utf16_t)))
            return NULL;

        return reinterpret_cast<lsp_utf16_t *>(pTemp->pData);
    }

    const lsp_utf16_t *LSPString::get_utf16le(ssize_t first, ssize_t last) const
    {
        XSAFE_TRANS(first, nLength, NULL);
        XSAFE_TRANS(last, nLength, NULL);
        if (first > last)
            return NULL;

        if (pTemp != NULL)
            pTemp->nOffset      = 0;

        lsp_utf16_t temp[BUF_SIZE + 8];
        lsp_utf16_t *th = temp, *tt = &temp[BUF_SIZE];

        for (ssize_t i=first; i<last; ++i)
        {
            lsp_wchar_t ch = pData[i];
            write_utf16le_codepoint(&th, ch);

            if (th >= tt)
            {
                if (!append_temp(reinterpret_cast<char *>(temp), (th - temp) * sizeof(lsp_utf16_t)))
                    return NULL;
                th  = temp;
            }
        }

        *(th++) = '\0';
        if (!append_temp(reinterpret_cast<char *>(temp), (th - temp) * sizeof(lsp_utf16_t)))
            return NULL;

        return reinterpret_cast<lsp_utf16_t *>(pTemp->pData);
    }

    const lsp_utf16_t *LSPString::get_utf16be(ssize_t first, ssize_t last) const
    {
        XSAFE_TRANS(first, nLength, NULL);
        XSAFE_TRANS(last, nLength, NULL);
        if (first > last)
            return NULL;

        if (pTemp != NULL)
            pTemp->nOffset      = 0;

        lsp_utf16_t temp[BUF_SIZE + 8];
        lsp_utf16_t *th = temp, *tt = &temp[BUF_SIZE];

        for (ssize_t i=first; i<last; ++i)
        {
            lsp_wchar_t ch = pData[i];
            write_utf16be_codepoint(&th, ch);

            if (th >= tt)
            {
                if (!append_temp(reinterpret_cast<char *>(temp), (th - temp) * sizeof(lsp_utf16_t)))
                    return NULL;
                th  = temp;
            }
        }

        *(th++) = '\0';
        if (!append_temp(reinterpret_cast<char *>(temp), (th - temp) * sizeof(lsp_utf16_t)))
            return NULL;

        return reinterpret_cast<lsp_utf16_t *>(pTemp->pData);
    }

    const char *LSPString::get_ascii(ssize_t first, ssize_t last) const
    {
        XSAFE_TRANS(first, nLength, NULL);
        XSAFE_TRANS(last, nLength, NULL);
        if (first > last)
            return NULL;

        if (!resize_temp(last - first + 1))
            return NULL;

        lsp_wchar_t *p  = &pData[first];
        char *dst       = pTemp->pData;

        for (; first < last; ++first)
        {
            lsp_wchar_t c   = *(p++);
            *(dst++)        = (c <= 0x7f) ? c : 0xff;
        }

        *(dst++)        = '\0';
        pTemp->nOffset  = dst - pTemp->pData;

        return pTemp->pData;
    }

#if defined(PLATFORM_WINDOWS)
    const char *LSPString::get_native(ssize_t first, ssize_t last, const char *charset) const
    {
        XSAFE_TRANS(first, nLength, NULL);
        XSAFE_TRANS(last, nLength, NULL);
        ssize_t length = last - first;
        if (length < 0)
            return NULL;
        else if (length == 0)
            return "";

        // Get codepage
        ssize_t cp = codepage_from_name(charset);
        if (cp < 0)
            return NULL;

        // Estimate number of bytes required
        lsp_utf16_t *buf    = const_cast<lsp_utf16_t *>(get_utf16(first, last));
        if (buf == NULL)
            return NULL;
        lsp_finally{ free(buf); };

        // Drop temporary data because it is stored in buf variable and can not be reused
        pTemp->pData        = NULL;
        pTemp->nLength      = 0;
        pTemp->nOffset      = 0;

        size_t slen         = length;
        ssize_t res         = widechar_to_multibyte(cp, buf, &slen, NULL, NULL) + 4; // + terminating 0
        if ((res <= 0) || (!resize_temp(res)))
            return NULL;

        // We have enough space for saving data
        size_t n            = res;
        res                 = widechar_to_multibyte(cp, buf, &slen, pTemp->pData, &n);
        if (res <= 0)
            return NULL;

        // Append terminating zero
        pTemp->pData[res++] = '\0';
        pTemp->pData[res++] = '\0';
        pTemp->pData[res++] = '\0';
        pTemp->pData[res++] = '\0';
        pTemp->nOffset      = res;

        return pTemp->pData;
    }
#else
    const char *LSPString::get_native(ssize_t first, ssize_t last, const char *charset) const
    {
        XSAFE_TRANS(first, nLength, NULL);
        XSAFE_TRANS(last, nLength, NULL);
        if (first > last)
            return NULL;

        // Open conversion
        iconv_t cd = init_iconv_from_wchar_t(charset);
        if (cd == iconv_t(-1))
            return get_utf8(first, last);

        // Analyze temp
        size_t outsize  = 0;
        char *outbuf    = NULL;
        if (pTemp != NULL)
        {
            pTemp->nOffset      = 0;
            outsize             = pTemp->nLength;
            outbuf              = pTemp->pData;
        }

        size_t insize   = (last - first) * sizeof(lsp_wchar_t);
        char *inbuf     = reinterpret_cast<char *>(const_cast<lsp_wchar_t *>(&pData[first]));

        while (insize > 0)
        {
            // Reserve space if there is not enough space
            if (outsize < 16)
            {
                // Try to grow the temprary buffer
                if (!grow_temp(BUF_SIZE))
                {
                    iconv_close(cd);
                    return NULL;
                }

                // Initialize location of buffers to store data
                outsize         = pTemp->nLength - pTemp->nOffset;
                outbuf          = &pTemp->pData[pTemp->nOffset];
            }

            // Do the conversion
            size_t nconv = iconv(cd, &inbuf, &insize, &outbuf, &outsize);
            if (nconv == (size_t) -1)
            {
                int err_code = errno;
                switch (err_code)
                {
                    case E2BIG:
                    case EINVAL:
                        break;
                    default:
                        iconv_close (cd);
                        return NULL;
                }
            }

            // Update pointer
            pTemp->nOffset      = pTemp->nLength - outsize;
        }

        // Close the iconv descriptor
        iconv_close(cd);

        // Append zeros at the end to make compatible with C-strings
        if (!append_temp("\x00\x00\x00\x00", 4))
            return NULL;

        return pTemp->pData;
    }
#endif /* PLATFORM_WINDOWS */

    size_t LSPString::match(const LSPString *s, size_t index) const
    {
        if (index >= nLength)
            return 0;
        size_t i=0, n = lsp_min(s->nLength, nLength - index);

        for (; i < n; ++i)
        {
            if (pData[i] != s->pData[i])
                return i;
        }
        return i;
    }

    size_t LSPString::match_nocase(const LSPString *s, size_t index) const
    {
        if (index >= nLength)
            return 0;
        size_t i=0, n = lsp_min(s->nLength, nLength - index);

        for (; i < n; ++i)
        {
            if (lsp::to_lower(pData[i]) != lsp::to_lower(s->pData[i]))
                return i;
        }
        return i;
    }

    char *LSPString::clone_utf8(size_t *bytes, ssize_t first, ssize_t last) const
    {
        const char *utf8 = get_utf8(first, last);
        if (utf8 == NULL)
            return NULL;

        size_t offset = (pTemp != NULL) ? pTemp->nOffset : 0;
        char *ptr = reinterpret_cast<char *>(lsp::memdup(utf8, offset));
        if (bytes != NULL)
            *bytes = (ptr != NULL) ? offset : 0;
        return ptr;
    }

    lsp_utf16_t *LSPString::clone_utf16(size_t *bytes, ssize_t first, ssize_t last) const
    {
        const lsp_utf16_t *utf16 = get_utf16(first, last);
        if (utf16 == NULL)
            return NULL;

        size_t offset = (pTemp != NULL) ? pTemp->nOffset : 0;
        lsp_utf16_t *ptr = reinterpret_cast<lsp_utf16_t *>(lsp::memdup(utf16, offset));
        if (bytes != NULL)
            *bytes = (ptr != NULL) ? offset : 0;
        return ptr;
    }

    lsp_utf16_t *LSPString::clone_utf16le(size_t *bytes, ssize_t first, ssize_t last) const
    {
        const lsp_utf16_t *utf16 = get_utf16le(first, last);
        if (utf16 == NULL)
            return NULL;

        size_t offset = (pTemp != NULL) ? pTemp->nOffset : 0;
        lsp_utf16_t *ptr = reinterpret_cast<lsp_utf16_t *>(lsp::memdup(utf16, offset));
        if (bytes != NULL)
            *bytes = (ptr != NULL) ? offset : 0;
        return ptr;
    }

    lsp_utf16_t *LSPString::clone_utf16be(size_t *bytes, ssize_t first, ssize_t last) const
    {
        const lsp_utf16_t *utf16 = get_utf16be(first, last);
        if (utf16 == NULL)
            return NULL;

        size_t offset = (pTemp != NULL) ? pTemp->nOffset : 0;
        lsp_utf16_t *ptr = reinterpret_cast<lsp_utf16_t *>(lsp::memdup(utf16, offset));
        if (bytes != NULL)
            *bytes = (ptr != NULL) ? offset : 0;
        return ptr;
    }

    char *LSPString::clone_ascii(size_t *bytes, ssize_t first, ssize_t last) const
    {
        const char *ascii = get_ascii(first, last);
        if (ascii == NULL)
            return NULL;

        size_t offset = (pTemp != NULL) ? pTemp->nOffset : 0;
        char *ptr = reinterpret_cast<char *>(lsp::memdup(ascii, offset));
        if (bytes != NULL)
            *bytes = (ptr != NULL) ? offset : 0;
        return ptr;
    }

    char *LSPString::clone_native(size_t *bytes, ssize_t first, ssize_t last, const char *charset) const
    {
        const char *native = get_native(first, last, charset);
        if (native == NULL)
            return NULL;

        size_t offset = (pTemp != NULL) ? pTemp->nOffset : 0;
        char *ptr = reinterpret_cast<char *>(lsp::memdup(native, offset));
        if (bytes != NULL)
            *bytes = (ptr != NULL) ? offset : 0;
        return ptr;
    }

    bool LSPString::append_temp(const char *p, size_t n) const
    {
        ssize_t free = (pTemp != NULL) ? pTemp->nLength - pTemp->nOffset : -1;

        if (free < ssize_t(n))
        {
            size_t resize   = n + (n >> 1);
            if (pTemp != NULL)
                resize         += pTemp->nLength;

            if (!resize_temp(resize))
                return false;
        }

        memcpy(&pTemp->pData[pTemp->nOffset], p, n * sizeof(char));
        pTemp->nOffset     += n;

        return true;
    }

    bool LSPString::grow_temp(size_t n) const
    {
        if (pTemp == NULL)
        {
            pTemp = static_cast<buffer_t *>(malloc(sizeof(buffer_t)));
            if (pTemp == NULL)
                return false;
            pTemp->nLength  = 0;
            pTemp->nOffset  = 0;
            pTemp->pData    = 0;
        }

        char *xc        = static_cast<char *>(realloc(pTemp->pData, (pTemp->nLength + n)*sizeof(char)));
        if (xc == NULL)
            return false;

        pTemp->pData    = xc;
        pTemp->nLength += n;
        return true;
    }

    bool LSPString::resize_temp(size_t n) const
    {
        if (pTemp == NULL)
        {
            pTemp = static_cast<buffer_t *>(malloc(sizeof(buffer_t)));
            if (pTemp == NULL)
                return false;
            pTemp->nLength  = 0;
            pTemp->nOffset  = 0;
            pTemp->pData    = 0;
        }

        char *xc        = static_cast<char *>(realloc(pTemp->pData, n*sizeof(char)));
        if (xc == NULL)
            return false;

        pTemp->pData    = xc;
        pTemp->nLength  = n;
        return true;
    }

    size_t LSPString::count(lsp_wchar_t ch) const
    {
        size_t n = 0;
        for (size_t i=0; i<nLength; ++i)
            if (pData[i] == ch)
                ++n;
        return n;
    }

    size_t LSPString::count(lsp_wchar_t ch, ssize_t first) const
    {
        XSAFE_TRANS(first, nLength, 0);

        size_t n = 0;
        for (size_t i=first; i<nLength; ++i)
            if (pData[i] == ch)
                ++n;
        return n;
    }

    size_t LSPString::count(lsp_wchar_t ch, ssize_t first, ssize_t last) const
    {
        XSAFE_TRANS(first, nLength, 0);
        XSAFE_TRANS(last, nLength, 0);

        size_t n = 0;
        if (first < last)
        {
            for (ssize_t i=first; i<last; ++i)
                if (pData[i] == ch)
                    ++n;
        }
        else
        {
            for (ssize_t i=last; i<first; ++i)
                if (pData[i] == ch)
                    ++n;
        }
        return n;
    }

    ssize_t LSPString::fmt_append_native(const char *fmt...)
    {
        LSPString tmp;
        va_list vl;
        va_start(vl, fmt);
        ssize_t res = tmp.vfmt_native(fmt, vl);
        va_end(vl);
        if (res < 0)
            return -res;
        return (append(&tmp)) ? res : -STATUS_NO_MEM;
    }

    ssize_t LSPString::fmt_preend_native(const char *fmt...)
    {
        LSPString tmp;
        va_list vl;
        va_start(vl, fmt);
        ssize_t res = tmp.vfmt_native(fmt, vl);
        va_end(vl);
        if (res < 0)
            return res;
        return (prepend(&tmp)) ? res : -STATUS_NO_MEM;
    }

    ssize_t LSPString::fmt_native(const char *fmt...)
    {
        va_list vl;
        va_start(vl, fmt);
        ssize_t res = vfmt_native(fmt, vl);
        va_end(vl);
        return res;
    }

    ssize_t LSPString::vfmt_append_native(const char *fmt, va_list args)
    {
        LSPString tmp;
        ssize_t res = tmp.vfmt_native(fmt, args);
        if (res < 0)
            return res;
        return (append(&tmp)) ? res : -STATUS_NO_MEM;
    }

    ssize_t LSPString::vfmt_preend_native(const char *fmt, va_list args)
    {
        LSPString tmp;
        ssize_t res = tmp.vfmt_native(fmt, args);
        if (res < 0)
            return res;
        return (prepend(&tmp)) ? res : -STATUS_NO_MEM;
    }

    ssize_t LSPString::vfmt_native(const char *fmt, va_list args)
    {
        char *ptr = NULL;
        ssize_t res = vasprintf(&ptr, fmt, args);
        if ((ptr == NULL) || (res < 0))
            return -STATUS_NO_MEM;
        if (!set_native(ptr, res))
            res     = -STATUS_NO_MEM;
        free(ptr);
        return res;
    }

    ssize_t LSPString::fmt_append_ascii(const char *fmt...)
    {
        LSPString tmp;
        va_list vl;
        va_start(vl, fmt);
        ssize_t res = tmp.vfmt_ascii(fmt, vl);
        va_end(vl);
        if (res < 0)
            return res;
        return (append(&tmp)) ? res : -STATUS_NO_MEM;
    }

    ssize_t LSPString::fmt_prepend_ascii(const char *fmt...)
    {
        LSPString tmp;
        va_list vl;
        va_start(vl, fmt);
        ssize_t res = tmp.vfmt_ascii(fmt, vl);
        va_end(vl);
        if (res < 0)
            return res;
        return (prepend(&tmp)) ? res : -STATUS_NO_MEM;
    }

    ssize_t LSPString::fmt_ascii(const char *fmt...)
    {
        va_list vl;
        va_start(vl, fmt);
        ssize_t res = vfmt_ascii(fmt, vl);
        va_end(vl);
        return res;
    }

    ssize_t LSPString::vfmt_append_ascii(const char *fmt, va_list args)
    {
        LSPString tmp;
        ssize_t res = tmp.vfmt_ascii(fmt, args);
        if (res < 0)
            return res;
        return (append(&tmp)) ? res : -STATUS_NO_MEM;
    }

    ssize_t LSPString::vfmt_prepend_ascii(const char *fmt, va_list args)
    {
        LSPString tmp;
        ssize_t res = tmp.vfmt_ascii(fmt, args);
        if (res < 0)
            return res;
        return (prepend(&tmp)) ? res : -STATUS_NO_MEM;
    }

    ssize_t LSPString::vfmt_ascii(const char *fmt, va_list args)
    {
        char *ptr = NULL;
        ssize_t res = vasprintf(&ptr, fmt, args);
        if ((ptr == NULL) || (res < 0))
            return -STATUS_NO_MEM;
        if (!set_ascii(ptr, res))
            res     = -STATUS_NO_MEM;
        free(ptr);
        return res;
    }

    ssize_t LSPString::fmt_append_utf8(const char *fmt...)
    {
        LSPString tmp;
        va_list vl;
        va_start(vl, fmt);
        ssize_t res = tmp.vfmt_utf8(fmt, vl);
        va_end(vl);
        if (res < 0)
            return res;
        return (append(&tmp)) ? res : -STATUS_NO_MEM;
    }

    ssize_t LSPString::fmt_prepend_utf8(const char *fmt...)
    {
        LSPString tmp;
        va_list vl;
        va_start(vl, fmt);
        ssize_t res = tmp.vfmt_utf8(fmt, vl);
        va_end(vl);
        if (res < 0)
            return res;
        return (prepend(&tmp)) ? res : -STATUS_NO_MEM;
    }

    ssize_t LSPString::fmt_utf8(const char *fmt...)
    {
        va_list vl;
        va_start(vl, fmt);
        ssize_t res = vfmt_utf8(fmt, vl);
        va_end(vl);
        return res;
    }

    ssize_t LSPString::vfmt_append_utf8(const char *fmt, va_list args)
    {
        LSPString tmp;
        ssize_t res = tmp.vfmt_utf8(fmt, args);
        if (res < 0)
            return res;
        return (append(&tmp)) ? res : -STATUS_NO_MEM;
    }

    ssize_t LSPString::vfmt_prepend_utf8(const char *fmt, va_list args)
    {
        LSPString tmp;
        ssize_t res = tmp.vfmt_utf8(fmt, args);
        if (res < 0)
            return res;
        return (prepend(&tmp)) ? res : -STATUS_NO_MEM;
    }

    ssize_t LSPString::vfmt_utf8(const char *fmt, va_list args)
    {
        char *ptr = NULL;
        ssize_t res = vasprintf(&ptr, fmt, args);
        if ((ptr == NULL) || (res < 0))
            return -STATUS_NO_MEM;
        if (!set_utf8(ptr, res))
            res     = -STATUS_NO_MEM;
        free(ptr);
        return res;
    }

    size_t LSPString::hash() const
    {
        if (pData == NULL)
            return 0;
        else if (nHash != 0)
            return nHash;

        size_t hash = 0;
        for (size_t i=0; i<nLength; ++i)
            hash = (hash * 0x10015) ^ pData[i];

        return nHash = hash;
    }

    bool LSPString::to_dos()
    {
        // Step 1: estimate the number of unprocessed line endings
        size_t count = 0;
        for (size_t i=0; i<nLength; ++i)
        {
            if (pData[i] != '\n')
                continue;
            if ((i == 0) || (pData[i-1] != '\r'))
                ++count;
        }

        // Return if there are no changes
        if (count <= 0)
            return true;
        else if (!cap_grow(count))
            return false;
        drop_temp();
        nHash               = 0;

        // Now we need to replace all line endings with proper ones
        size_t distance;
        lsp_wchar_t *dst    = &pData[nLength + count];
        const lsp_wchar_t *src = &pData[nLength];
        const lsp_wchar_t *blk;
        nLength            += count;

        while (count-- > 0)
        {
            // Find the occurrence
            for (blk = src-1; blk > pData; --blk)
            {
                if (*blk != '\n')
                    continue;
                if (blk[-1] != '\r')
                    break;
            }

            // blk now points at the block before the '\r' symbol
            distance        = src - blk;
            dst            -= distance;
            if (distance > 0)
                memmove(dst, blk, distance * sizeof(lsp_wchar_t));
            src             = blk;
            *(--dst)        = '\r';
        }

        return true;
    }

    bool LSPString::to_unix()
    {
        // Step 1: estimate the number of unprocessed line endings
        size_t count = 0;
        for (size_t i=1; i<nLength; ++i)
        {
            if (pData[i] != '\n')
                continue;
            if (pData[i-1] == '\r')
                ++count;
        }

        // Return if there are no changes
        if (count <= 0)
            return true;
        drop_temp();
        nHash                   = 0;

        size_t distance;
        lsp_wchar_t *dst        = pData;
        const lsp_wchar_t *src  = pData;
        const lsp_wchar_t *end  = &pData[nLength];
        const lsp_wchar_t *blk;
        nLength                -= count;

        while (count-- > 0)
        {
            // Find the occurrence
            for (blk = src + 1; blk < end; ++blk)
            {
                if (*blk != '\n')
                    continue;
                if ((blk > pData) && (blk[-1] == '\r'))
                    break;
            }

            // blk now points at the end of block before the '\r' symbol
            distance        = blk - src - 1;
            if ((distance > 0) && (src != dst))
                memmove(dst, src, distance * sizeof(lsp_wchar_t));
            src            += distance + 1;
            dst            += distance;
        }

        // move the rest data
        if (src < end)
            memmove(dst, src, (end - src) * sizeof(lsp_wchar_t));

        return true;
    }

    namespace lltl
    {
        size_t hash_spec<LSPString>::hash_func(const void *ptr, size_t size)
        {
            return (static_cast<const LSPString *>(ptr))->hash();
        }

        ssize_t compare_spec<LSPString>::cmp_func(const void *a, const void *b, size_t size)
        {
            const LSPString *sa = static_cast<const LSPString *>(a);
            const LSPString *sb = static_cast<const LSPString *>(b);
            return sa->compare_to(sb);
        }

        void *allocator_spec<LSPString>::clone_func(const void *src, size_t size)
        {
            return (static_cast<const LSPString *>(src))->clone();
        }

        void allocator_spec<LSPString>::free_func(void *ptr)
        {
            delete (static_cast<LSPString *>(ptr));
        }
    }

} /* namespace lsp */
