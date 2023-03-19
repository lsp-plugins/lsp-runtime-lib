/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-runtime-lib
 * Created on: 13 февр. 2019 г.
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
#include <lsp-plug.in/test-fw/utest.h>
#include <lsp-plug.in/common/endian.h>

#include <lsp-plug.in/io/charset.h>

// This strlen does not analyze surrogate pairs
namespace lsp
{
    template <class char_t>
        static size_t strlen_test(const char_t *s)
        {
            size_t n = 0;
            for (; *s != 0; ++s, ++n) {}
            return n;
        }

    static size_t utf16_count_invalid(const lsp_utf16_t *s)
    {
        size_t n = 0;
        for (; *s != 0; ++s) {
            if (*s == 0xfffd)
                ++n;
        }
        return n;
    }

    static size_t utf32_count_invalid(const lsp_utf32_t *s)
    {
        size_t n = 0;
        for (; *s != 0; ++s) {
            if (*s == 0xfffd)
                ++n;
        }
        return n;
    }

    static size_t utf8_count_invalid(const char *s)
    {
        lsp_utf32_t cp;
        size_t n = 0;

        while ((cp = lsp::read_utf8_codepoint(&s)) != 0)
        {
            if (cp == 0xfffd)
                ++n;
        }
        return n;
    }

    template <class char_t>
        static char_t *strdup_bswap(const char_t *src, bool le)
        {
            // Estimate string length
            size_t len = 0;
            for (const char_t *p = src; *p != 0; ++p)
                ++len;

            // Allocate memory
            char_t *dst = reinterpret_cast<char_t *>(::malloc(sizeof(char_t) * (len + 1)));
            if (dst == NULL)
                return NULL;

            // Do byte-reversed copy (if required)
            bool swap = __IF_LEBE(!le, le);
            char_t *p = dst;

            if (swap)
            {
                while (*src != 0)
                    *(p++) = byte_swap(*(src++));
            }
            else
            {
                while (*src != 0)
                *(p++) = *(src++);
            }
            *p = 0;

            return dst;
        }

    template <class char_t>
        static ssize_t strcmp_test(const char_t *s1, const char_t *s2)
        {
            ssize_t diff;
            do {
                diff = ssize_t(*s1) - ssize_t(*s2);
            } while ((diff == 0) && (*(s1++) != 0) && (*(s2++) != 0));

            return diff;
        }

    template <class char_t>
        static ssize_t strcmp_bswap(const char_t *s1, const char_t *s2, bool le)
        {
            ssize_t diff;

            bool swap = __IF_LEBE(!le, le);
            if (swap)
            {
                do {
                    diff = ssize_t(*s1) - ssize_t(byte_swap(*s2));
                } while ((diff == 0) && (*(s1++) != 0) && (*(s2++) != 0));
            }
            else
            {
                do {
                    diff = ssize_t(*s1) - ssize_t(*s2);
                } while ((diff == 0) && (*(s1++) != 0) && (*(s2++) != 0));
            }

            return diff;
        }

    typedef struct utf8_check_t
    {
        const char *s;
        size_t u16strlen;
        size_t u32strlen;
        size_t invalid;
    } utf8_check_t;

    typedef struct utf16_check_t
    {
        const uint16_t *s;
        size_t u8strlen;
        size_t u32strlen;
        size_t invalid;
    } utf16_check_t;

    static utf8_check_t utf8_check[] =
    {
        { "", 0, 0, 0 },
        { "Test text in ASCII", 18, 18, 0 },
        { "Тестовый текст на русском языке", 31, 31, 0 },
        { "日本語の物語。それはテスト。", 14, 14, 0 },
        { "Test text in ASCII. Тестовый текст на русском языке. 日本語の物語。それはテスト。", 67, 67, 0 },
        { "\x80", 1, 1, 1 },                       // Invalid sequence
        { "\x80Test", 5, 5, 1 },                   // Invalid sequence + text
        { "\xe0\x80\x80", 3, 3, 3 },               // Invalid sequence
        { "\xe0\x80\x80Test", 7, 7, 3 },           // Invalid sequence + text
        { "\xe0\x80\x80\x80", 4, 4, 4 },           // Two invalid sequences
        { "\xe0\x80\x80\x80Test", 8, 8, 4 },       // Two invalid sequences + text
        { "\xed\xa0\x80", 1, 1, 1 },               // Invalid codepoint (surrogate)
        { "\xed\xa0\x80Test", 5, 5, 1 },           // Invalid codepoint (surrogate) + text
        { "\xed\xa0\x80\xed\xa0\x8f", 2, 2, 2 },   // Two invalid codepoints (surrogate)
        { "\xed\xa0\x80\xed\xa0\x8fTest", 6, 6, 2 },   // Two invalid codepoints (surrogate) + text
        { "\xc0\xbf\xcb\xbf", 3, 3, 2 },           // One valid codepoint, one invalid (2 errors)
        { "\xc0\xbf\xcb\xbfTest", 7, 7, 2 },       // One valid codepoint, one invalid (2 errors) + text
        { "\xf0\x90\x80\x8f", 2, 1, 0 },           // Surrogate pair at output
        { "\xf0\x90\x80\x8fTest", 6, 5, 0 }        // Surrogate pair at output + text
    };

    static const uint16_t u16str_0[]    = { 0xd801, 0xdc37, 0 };
    static const uint16_t u16str_1[]    = { 0xd801, 0xdc37, 'T', 'e', 's', 't', 0 };
    static const uint16_t u16str_2[]    = { 0xdc37, 0xd801, 0 };
    static const uint16_t u16str_3[]    = { 0xdc37, 0xd801, 'T', 'e', 's', 't', 0 };
    static const uint16_t u16str_4[]    = { 0xd801, 0 };
    static const uint16_t u16str_5[]    = { 0xd801, 'T', 'e', 's', 't', 0 };
    static const uint16_t u16str_6[]    = { 0xdc01, 0 };
    static const uint16_t u16str_7[]    = { 0xdc01, 'T', 'e', 's', 't', 0 };
    static const uint16_t u16str_8[]    = { 0 };
    static const uint16_t u16str_9[]    = { 'T', 'e', 's', 't', ' ', 't', 'e', 'x', 't', ' ', 'i', 'n', ' ', 'A', 'S', 'C', 'I', 'I', 0 };
    static const uint16_t u16str_10[]   = {
        0x0422, 0x0435, 0x0441, 0x0442, 0x043e, 0x0432, 0x044b, 0x0439,
        0x0020, 0x0442, 0x0435, 0x043a, 0x0441, 0x0442, 0x0020, 0x043d,
        0x0430, 0x0020, 0x0440, 0x0443, 0x0441, 0x0441, 0x043a, 0x043e,
        0x043c, 0x0020, 0x044f, 0x0437, 0x044b, 0x043a, 0x0435, 0
    };

    static const uint16_t u16str_11[]   = {
        0x65e5, 0x672c, 0x8a9e, 0x306e, 0x7269, 0x8a9e, 0x3002, 0x305d,
        0x308c, 0x306f, 0x30c6, 0x30b9, 0x30c8, 0x3002, 0
    };

    static const uint16_t u16str_12[]   = {
        'T', 'e', 's', 't', ' ', 't', 'e', 'x', 't', ' ', 'i', 'n', ' ', 'A', 'S', 'C', 'I', 'I',
        '.', ' ',
        0x0422, 0x0435, 0x0441, 0x0442, 0x043e, 0x0432, 0x044b, 0x0439,
        0x0020, 0x0442, 0x0435, 0x043a, 0x0441, 0x0442, 0x0020, 0x043d,
        0x0430, 0x0020, 0x0440, 0x0443, 0x0441, 0x0441, 0x043a, 0x043e,
        0x043c, 0x0020, 0x044f, 0x0437, 0x044b, 0x043a, 0x0435,
        '.', ' ',
        0x65e5, 0x672c, 0x8a9e, 0x306e, 0x7269, 0x8a9e, 0x3002, 0x305d,
        0x308c, 0x306f, 0x30c6, 0x30b9, 0x30c8, 0x3002,
        0
    };

    static utf16_check_t utf16_check[] =
    {
        { u16str_0, 4, 1, 0 },                     // Surrogate pair
        { u16str_1, 8, 5, 0 },                     // Surrogate pair + text
        { u16str_2, 4, 1, 0 },                     // Another surrogate pair
        { u16str_3, 8, 5, 0 },                     // Another surrogate pair + text
        { u16str_4, 3, 1, 1 },                     // Invalid surrogate
        { u16str_5, 7, 5, 1 },                     // Invalid surrogate + text
        { u16str_6, 3, 1, 1 },                     // Another invalid surrogate
        { u16str_7, 7, 5, 1 },                     // Another invalid surrogate + text
        { u16str_8, 0, 0, 0 },                     // ""
        { u16str_9, 18, 18, 0 },                   // "Test text in ASCII"
        { u16str_10, 58, 31, 0 },                  // "Тестовый текст на русском языке"
        { u16str_11, 42, 14, 0 },                  // "日本語の物語。それはテスト。"
        { u16str_12, 122, 67, 0 }                  // "Test text in ASCII. Тестовый текст на русском языке. 日本語の物語。それはテスト。"
    };
}

UTEST_BEGIN("runtime.io", charset)

    void check_utf8_to_utfX()
    {
        lsp_utf16_t *s_na = NULL, *s_le = NULL, *s_be = NULL;
        lsp_utf32_t *s32_na = NULL, *s32_le = NULL, *s32_be = NULL;
        size_t len;

        printf("Testing check_utf8_to_utfX\n");

        for (size_t i=0, n=sizeof(utf8_check)/sizeof(utf8_check_t); i<n; ++i)
        {
            printf("  checking test line %d...\n", int(i));

            utf8_check_t *ck = &utf8_check[i];

            // UTF8 -> UTF16
            s_na   = lsp::utf8_to_utf16(ck->s);
            UTEST_ASSERT(s_na != NULL);
            lsp_finally{ free(s_na); };

            s_le   = lsp::utf8_to_utf16le(ck->s);
            UTEST_ASSERT(s_le != NULL);
            lsp_finally{ free(s_le); };

            s_be   = lsp::utf8_to_utf16be(ck->s);
            UTEST_ASSERT(s_be != NULL);
            lsp_finally{ free(s_be); };

            UTEST_ASSERT_MSG(((len = strlen_test(s_na)) == ck->u16strlen),
                    "Error checking line %d: utf16_strlen=%d, expected=%d",
                    int(i), int(len), int(ck->u16strlen));
            UTEST_ASSERT_MSG(((len = utf16_count_invalid(s_na)) == ck->invalid),
                    "Error checking line %d: utf16_count_invalid=%d, expected=%d",
                    int(i), int(len), int(ck->invalid));

            UTEST_ASSERT(strcmp_bswap(s_na, s_le, true) == 0);
            UTEST_ASSERT(strcmp_bswap(s_na, s_be, false) == 0);
            if (len > 0)
                UTEST_ASSERT(strcmp_test(s_le, s_be) != 0);

            // UTF8 -> UTF32
            s32_na = lsp::utf8_to_utf32(ck->s);
            UTEST_ASSERT(s32_na != NULL);
            lsp_finally{ free(s32_na); };

            s32_le = lsp::utf8_to_utf32le(ck->s);
            UTEST_ASSERT(s32_le != NULL);
            lsp_finally{ free(s32_le); };

            s32_be = lsp::utf8_to_utf32be(ck->s);
            UTEST_ASSERT(s32_be != NULL);
            lsp_finally{ free(s32_be); };

            UTEST_ASSERT_MSG(((len = strlen_test(s32_na)) == ck->u32strlen),
                    "Error checking line %d: utf32_strlen=%d, expected=%d",
                    int(i), int(len), int(ck->u32strlen));
            UTEST_ASSERT_MSG(((len = utf32_count_invalid(s32_na)) == ck->invalid),
                    "Error checking line %d: utf32_count_invalid=%d, expected=%d",
                    int(i), int(len), int(ck->invalid));

            UTEST_ASSERT(strcmp_bswap(s32_na, s32_le, true) == 0);
            UTEST_ASSERT(strcmp_bswap(s32_na, s32_be, false) == 0);
            if (len > 0)
                UTEST_ASSERT(strcmp_test(s32_le, s32_be) != 0);
        }
    }

    void check_utf16_to_utfX()
    {
        lsp_utf16_t *le = NULL, *be = NULL;
        const lsp_utf16_t *na = NULL;
        char *s_na = NULL, *s_le = NULL, *s_be = NULL;
        lsp_utf32_t *s32[9];
        size_t len;
        printf("Testing check_utf16_to_utfX\n");

        for (size_t i=0, n=sizeof(utf16_check)/sizeof(utf16_check_t); i<n; ++i)
        {
            printf("  checking test line %d...\n", int(i));

            utf16_check_t *ck = &utf16_check[i];

            // Obtain native string and it's LE and BE copies
            na  = reinterpret_cast<const lsp_utf16_t *>(ck->s);
            le  = strdup_bswap(na, true);
            be  = strdup_bswap(na, false);

            len = strlen_test(na);
            UTEST_ASSERT(le != NULL);
            UTEST_ASSERT(be != NULL);
            if (len > 0)
                UTEST_ASSERT(strcmp_test(le, be) != 0);

            // UTF16 -> UTF8
            s_na    = lsp::utf16_to_utf8(na);
            s_le    = lsp::utf16le_to_utf8(le);
            s_be    = lsp::utf16be_to_utf8(be);

            UTEST_ASSERT(s_na != NULL);
            UTEST_ASSERT(s_le != NULL);
            UTEST_ASSERT(s_be != NULL);

            UTEST_ASSERT_MSG(((len = strlen_test(s_na)) == ck->u8strlen),
                    "Error checking line %d: strlen=%d, expected=%d",
                    int(i), int(len), int(ck->u8strlen));
            UTEST_ASSERT_MSG(((len = utf8_count_invalid(s_na)) == ck->invalid),
                    "Error checking line %d: utf8_count_invalid=%d, expected=%d",
                    int(i), int(len), int(ck->invalid));

            UTEST_ASSERT(strcmp_test(s_le, s_be) == 0);
            UTEST_ASSERT(strcmp_test(s_na, s_le) == 0);
            UTEST_ASSERT(strcmp_test(s_na, s_be) == 0);

            free(s_na);
            free(s_le);
            free(s_be);

            // UTF16 -> UTF32
            s32[0]  = lsp::utf16_to_utf32(na);
            s32[1]  = lsp::utf16le_to_utf32(le);
            s32[2]  = lsp::utf16be_to_utf32(be);
            s32[3]  = lsp::utf16_to_utf32le(na);
            s32[4]  = lsp::utf16le_to_utf32le(le);
            s32[5]  = lsp::utf16be_to_utf32le(be);
            s32[6]  = lsp::utf16_to_utf32be(na);
            s32[7]  = lsp::utf16le_to_utf32be(le);
            s32[8]  = lsp::utf16be_to_utf32be(be);

            for (size_t j=0; j<9; ++j)
                UTEST_ASSERT(s32[j] != NULL);

            UTEST_ASSERT_MSG(((len = strlen_test(s32[0])) == ck->u32strlen),
                    "Error checking line %d: strlen=%d, expected=%d",
                    int(i), int(len), int(ck->u32strlen));
            UTEST_ASSERT_MSG(((len = utf32_count_invalid(s32[0])) == ck->invalid),
                    "Error checking line %d: utf32_count_invalid=%d, expected=%d",
                    int(i), int(len), int(ck->invalid));

            UTEST_ASSERT(strcmp_test(s32[0], s32[1]) == 0);
            UTEST_ASSERT(strcmp_test(s32[0], s32[2]) == 0);
            UTEST_ASSERT(strcmp_bswap(s32[0], s32[3], true) == 0);
            UTEST_ASSERT(strcmp_bswap(s32[0], s32[4], true) == 0);
            UTEST_ASSERT(strcmp_bswap(s32[0], s32[5], true) == 0);
            UTEST_ASSERT(strcmp_bswap(s32[0], s32[6], false) == 0);
            UTEST_ASSERT(strcmp_bswap(s32[0], s32[7], false) == 0);
            UTEST_ASSERT(strcmp_bswap(s32[0], s32[8], false) == 0);

            if (len > 0)
            {
                UTEST_ASSERT(strcmp_test(s32[4], s32[7]) != 0);
                UTEST_ASSERT(strcmp_test(s32[5], s32[7]) != 0);
                UTEST_ASSERT(strcmp_test(s32[4], s32[8]) != 0);
                UTEST_ASSERT(strcmp_test(s32[5], s32[8]) != 0);
            }

            for (size_t j=0; j<9; ++j)
                free(s32[j]);

            // Free LE and BE copies
            free(le);
            free(be);
        }
    }

    void check_latin_lower_upper()
    {
        static const lsp_wchar_t lower[] = {
            'a', 'b', 'c', 'd',
            'e', 'f', 'g', 'h',
            'i', 'j', 'k', 'l',
            'm', 'n', 'o', 'p',
            'q', 'r', 's', 't',
            'u', 'v', 'w', 'x',
            'y', 'z'
        };

        static const lsp_wchar_t upper[] = {
            'A', 'B', 'C', 'D',
            'E', 'F', 'G', 'H',
            'I', 'J', 'K', 'L',
            'M', 'N', 'O', 'P',
            'Q', 'R', 'S', 'T',
            'U', 'V', 'W', 'X',
            'Y', 'Z'
        };

        printf("Testing lsp::to_lower() and lsp::to_upper() for latin character set\n");

        for (size_t i=0; i<sizeof(lower)/sizeof(lower[0]); ++i)
        {
            lsp_wchar_t lc = lsp::to_lower(upper[i]);
            lsp_wchar_t uc = lsp::to_upper(lower[i]);

            UTEST_ASSERT_MSG(
                lc == lower[i],
                "Failed conversion of character 0x%x to lower value 0x%x, result value is 0x%x (index=%d)",
                int(upper[i]), int(lower[i]), int(lc), int(i));
            UTEST_ASSERT_MSG(
                uc == upper[i],
                "Failed conversion of character 0x%x to upper value 0x%x, result value is 0x%x (index=%d)",
                int(lower[i]), int(upper[i]), int(uc), int(i));
        }
    }

    void check_cyrillic_lower_upper()
    {
        static const lsp_wchar_t lower[] = {
            0x450, 0x451, 0x452, 0x453,
            0x454, 0x455, 0x456, 0x457,
            0x458, 0x459, 0x45a, 0x45b,
            0x45c, 0x45d, 0x45e, 0x45f,

            0x430, 0x431, 0x432, 0x433,
            0x434, 0x435, 0x436, 0x437,
            0x438, 0x439, 0x43a, 0x43b,
            0x43c, 0x43d, 0x43e, 0x43f,
            0x440, 0x441, 0x442, 0x443,
            0x444, 0x445, 0x446, 0x447,
            0x448, 0x449, 0x44a, 0x44b,
            0x44c, 0x44d, 0x44e, 0x44f,

            0x461, 0x463, 0x465, 0x467,
            0x469, 0x46b, 0x46d, 0x46f,
            0x471, 0x473, 0x475, 0x477,
            0x479, 0x47b, 0x47d, 0x47f,
            0x481,

            0x482, 0x483, 0x484, 0x485,
            0x486, 0x487, 0x488, 0x489,

            0x48b, 0x48d, 0x48f,
            0x491, 0x493, 0x495, 0x497,
            0x499, 0x49b, 0x49d, 0x49f,
            0x4a1, 0x4a3, 0x4a5, 0x4a7,
            0x4a9, 0x4ab, 0x4ad, 0x4af,
            0x4b1, 0x4b3, 0x4b5, 0x4b7,
            0x4b9, 0x4bb, 0x4bd, 0x4bf,
            0x4c1, 0x4c3, 0x4c5, 0x4c7,
            0x4c9, 0x4cb, 0x4cd, 0x4cf,
            0x4d1, 0x4d3, 0x4d5, 0x4d7,
            0x4d9, 0x4db, 0x4dd, 0x4df,
            0x4e1, 0x4e3, 0x4e5, 0x4e7,
            0x4e9, 0x4eb, 0x4ed, 0x4ef,
            0x4f1, 0x4f3, 0x4f5, 0x4f7,
            0x4f9, 0x4fb, 0x4fd, 0x4ff,

            0x501, 0x503, 0x505, 0x507,
            0x509, 0x50b, 0x50d, 0x50f,
            0x511, 0x513, 0x515, 0x517,
            0x519, 0x51b, 0x51d, 0x51f,
            0x521, 0x523, 0x525, 0x527,
            0x529, 0x52b, 0x52d, 0x52f,

            0xa641, 0xa643, 0xa645, 0xa647,
            0xa649, 0xa64b, 0xa64d, 0xa64f,
            0xa651, 0xa653, 0xa655, 0xa657,
            0xa659, 0xa65b, 0xa65d, 0xa65f,
            0xa661, 0xa663, 0xa665, 0xa667,
            0xa669, 0xa66b, 0xa66d, 0xa66e,

            0xa681, 0xa683, 0xa685, 0xa687,
            0xa689, 0xa68b, 0xa68d, 0xa68f,
            0xa691, 0xa693, 0xa695, 0xa697,
            0xa699, 0xa69b, 0xa69c, 0xa69e,
        };

        static const lsp_wchar_t upper[] = {
            0x400, 0x401, 0x402, 0x403,
            0x404, 0x405, 0x406, 0x407,
            0x408, 0x409, 0x40a, 0x40b,
            0x40c, 0x40d, 0x40e, 0x40f,

            0x410, 0x411, 0x412, 0x413,
            0x414, 0x415, 0x416, 0x417,
            0x418, 0x419, 0x41a, 0x41b,
            0x41c, 0x41d, 0x41e, 0x41f,
            0x420, 0x421, 0x422, 0x423,
            0x424, 0x425, 0x426, 0x427,
            0x428, 0x429, 0x42a, 0x42b,
            0x42c, 0x42d, 0x42e, 0x42f,

            0x460, 0x462, 0x464, 0x466,
            0x468, 0x46a, 0x46c, 0x46e,
            0x470, 0x472, 0x474, 0x476,
            0x478, 0x47a, 0x47c, 0x47e,
            0x480,

            0x482, 0x483, 0x484, 0x485,
            0x486, 0x487, 0x488, 0x489,

            0x48a, 0x48c, 0x48e,
            0x490, 0x492, 0x494, 0x496,
            0x498, 0x49a, 0x49c, 0x49e,
            0x4a0, 0x4a2, 0x4a4, 0x4a6,
            0x4a8, 0x4aa, 0x4ac, 0x4ae,
            0x4b0, 0x4b2, 0x4b4, 0x4b6,
            0x4b8, 0x4ba, 0x4bc, 0x4be,
            0x4c0, 0x4c2, 0x4c4, 0x4c6,
            0x4c8, 0x4ca, 0x4cc, 0x4ce,
            0x4d0, 0x4d2, 0x4d4, 0x4d6,
            0x4d8, 0x4da, 0x4dc, 0x4de,
            0x4e0, 0x4e2, 0x4e4, 0x4e6,
            0x4e8, 0x4ea, 0x4ec, 0x4ee,
            0x4f0, 0x4f2, 0x4f4, 0x4f6,
            0x4f8, 0x4fa, 0x4fc, 0x4fe,

            0x500, 0x502, 0x504, 0x506,
            0x508, 0x50a, 0x50c, 0x50e,
            0x510, 0x512, 0x514, 0x516,
            0x518, 0x51a, 0x51c, 0x51e,
            0x520, 0x522, 0x524, 0x526,
            0x528, 0x52a, 0x52c, 0x52e,

            0xa640, 0xa642, 0xa644, 0xa646,
            0xa648, 0xa64a, 0xa64c, 0xa64e,
            0xa650, 0xa652, 0xa654, 0xa656,
            0xa658, 0xa65a, 0xa65c, 0xa65e,
            0xa660, 0xa662, 0xa664, 0xa666,
            0xa668, 0xa66a, 0xa66c, 0xa66e,

            0xa680, 0xa682, 0xa684, 0xa686,
            0xa688, 0xa68a, 0xa68c, 0xa68e,
            0xa690, 0xa692, 0xa694, 0xa696,
            0xa698, 0xa69a, 0xa69c, 0xa69e,
        };

        printf("Testing lsp::to_lower() and lsp::to_upper() for cyrillic character set\n");

        for (size_t i=0; i<sizeof(lower)/sizeof(lower[0]); ++i)
        {
            lsp_wchar_t lc = lsp::to_lower(upper[i]);
            lsp_wchar_t uc = lsp::to_upper(lower[i]);

            UTEST_ASSERT_MSG(
                lc == lower[i],
                "Failed conversion of character 0x%x to lower value 0x%x, result value is 0x%x (index=%d)",
                int(upper[i]), int(lower[i]), int(lc), int(i));
            UTEST_ASSERT_MSG(
                uc == upper[i],
                "Failed conversion of character 0x%x to upper value 0x%x, result value is 0x%x (index=%d)",
                int(lower[i]), int(upper[i]), int(uc), int(i));
        }
    }

    UTEST_MAIN
    {
        check_utf8_to_utfX();
        check_utf16_to_utfX();
//        check_latin_lower_upper();
        check_cyrillic_lower_upper();
    }
UTEST_END;



