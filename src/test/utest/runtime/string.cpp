/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-runtime-lib
 * Created on: 28 авг. 2018 г.
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

#include <lsp-plug.in/test-fw/utest.h>
#include <lsp-plug.in/runtime/LSPString.h>
#include <lsp-plug.in/lltl/pphash.h>

namespace lsp
{
    static const lsp_utf16_t utf16_ja[] =
    {
        0x6DBC, 0x5BAE, 0x30CF, 0x30EB, 0x30D2, 0x306E, 0x6182, 0x9B31, 0xFF1A, 0x7B2C, 0x4E00, 0x7AE0, 0x002E,
        0
    };

    static const lsp_utf16_t utf16_ru[] =
    {
        0x0412, 0x0441, 0x0435, 0x043C, 0x0020, 0x043F, 0x0440, 0x0438, 0x0432, 0x0435, 0x0442, 0x0021,
        0
    };

    static const lsp_utf16_t utf16_ja_inv[] =
    {
        0x6DBC, 0x5BAE, 0x30CF, 0x30EB, 0x30D2, 0x306E, 0x6182, 0x9B31, 0xFF1A, 0x7B2C, 0x4E00, 0x7AE0, 0x002E,
        0xDC00, 0
    };

    static const lsp_utf16_t utf16_ru_inv[] =
    {
        0x0412, 0x0441, 0x0435, 0x043C, 0x0020, 0x043F, 0x0440, 0x0438, 0x0432, 0x0435, 0x0442, 0x0021,
        0xD800, 0
    };
}

UTEST_BEGIN("runtime.runtime", string)

    void test_basic()
    {
        LSPString s1, s2, s3, s4, s5, s6, s7;

        printf("Performing basic test...\n");

        // Settings
        UTEST_ASSERT(s1.set_ascii("This is some text"));
        UTEST_ASSERT(s1.get_native() != NULL);
        printf("s1 = %s\n", s1.get_native());

        UTEST_ASSERT(s2.set_utf8("Всем привет!"));
        UTEST_ASSERT(s2.get_native() != NULL);
        printf("s2 = %s\n", s2.get_native());

        UTEST_ASSERT(s3.set_utf8("涼宮ハルヒの憂鬱：第一章."));
        UTEST_ASSERT(s3.get_native() != NULL);
        printf("s3 = %s\n", s3.get_native());

        UTEST_ASSERT(s4.set_ascii("Всем привет!"));
        UTEST_ASSERT(s4.get_native() != NULL);
        printf("s4 = %s\n", s4.get_native());

        UTEST_ASSERT(s5.set(&s1));
        UTEST_ASSERT(s5.get_native() != NULL);
        printf("s5 = %s\n", s5.get_native());

        UTEST_ASSERT(s6.set_utf16(utf16_ja));
        UTEST_ASSERT(s6.get_native() != NULL);
        printf("s6 = %s\n", s6.get_native());

        UTEST_ASSERT(s7.set_utf16(utf16_ru));
        UTEST_ASSERT(s7.get_native() != NULL);
        printf("s7 = %s\n", s7.get_native());

        UTEST_ASSERT(s1.length() == 17);
        UTEST_ASSERT(s2.length() == 12);
        UTEST_ASSERT(s3.length() == 13);
        UTEST_ASSERT(s4.length() == 22);
        UTEST_ASSERT(s5.length() == s1.length());
        UTEST_ASSERT(s6.length() == 13);
        UTEST_ASSERT(s7.length() == 12);
        UTEST_ASSERT(s2.equals(&s7));
        UTEST_ASSERT(s3.equals(&s6));

        // Test data coding
        UTEST_ASSERT(s6.set_utf8("涼宮ハルヒの憂鬱：第一章."));
        UTEST_ASSERT(s7.set_utf16(s6.get_utf16()));
        UTEST_ASSERT(s6.compare_to(&s7) == 0);
        s7.clear();
        UTEST_ASSERT(s7.set_utf8(s6.get_utf8()));
        UTEST_ASSERT(s6.compare_to(&s7) == 0);

        UTEST_ASSERT(s6.set_utf8("Всем привет!"));
        UTEST_ASSERT(s7.set_utf16(s6.get_utf16()));
        UTEST_ASSERT(s6.equals(&s7));
        s7.clear();
        UTEST_ASSERT(s7.set_utf8(s6.get_utf8()));
        UTEST_ASSERT(s6.equals(&s7));

        UTEST_ASSERT(s6.set_utf8("This is some text"));
        UTEST_ASSERT(s7.set_utf16(s6.get_utf16()));
        UTEST_ASSERT(s6.equals(&s7));
        s7.clear();
        UTEST_ASSERT(s7.set_utf8(s6.get_utf8()));
        UTEST_ASSERT(s6.equals(&s7));

        // Test some invalid sequences
        UTEST_ASSERT(s6.set_utf8("涼宮ハルヒの憂鬱：第一章.\xff"));
        UTEST_ASSERT(s6.length() == 14);
        UTEST_ASSERT(s6.last() == 0xfffd);
        UTEST_ASSERT(s6.get_native() != NULL);
        printf("s6 = %s\n", s6.get_native());

        UTEST_ASSERT(s6.set_utf16(utf16_ja_inv));
        UTEST_ASSERT(s6.length() == 14);
        UTEST_ASSERT(s6.last() == 0xfffd);
        UTEST_ASSERT(s6.get_native() != NULL);
        printf("s6 = %s\n", s6.get_native());

        UTEST_ASSERT(s6.set_utf16(utf16_ru_inv));
        UTEST_ASSERT(s6.length() == 13);
        UTEST_ASSERT(s6.last() == 0xfffd);
        UTEST_ASSERT(s6.get_native() != NULL);
        printf("s6 = %s\n", s6.get_native());

        // Perform operations
        UTEST_ASSERT(s3.set(&s1, 8, 12)); // "some"
        UTEST_ASSERT(s3.get_native() != NULL);
        printf("s3 = %s\n", s3.get_native());

        UTEST_ASSERT(s4.set(&s1, -9, 12)); // "some"
        UTEST_ASSERT(s4.get_native() != NULL);
        printf("s4 = %s\n", s4.get_native());

        UTEST_ASSERT(s5.set(&s1, -4, -10)); // empty because -5 < -9
        UTEST_ASSERT(s5.is_empty());
        UTEST_ASSERT(s5.get_native() != NULL);
        printf("s5 = %s\n", s5.get_native());
        UTEST_ASSERT(s3.equals(&s4));

        UTEST_ASSERT(s5.length() == 0);

        s3.toupper(); // "SOME"
        printf("s3 = %s\n", s3.get_native());
        UTEST_ASSERT(s3.get_native() != NULL);

        s1.toupper(); // "THIS IS SOME TEXT"
        UTEST_ASSERT(s1.get_native() != NULL);
        printf("s1 = %s\n", s1.get_native());

        s4.clear(); // ""
        UTEST_ASSERT(s4.get_native() != NULL);
        printf("s4 = %s\n", s4.get_native());

        UTEST_ASSERT(s4.is_empty());
        UTEST_ASSERT(s1.index_of(&s3) == 8);
        s1.swap(&s3); // s1 = "SOME", s3 = "THIS IS SOME TEXT"

        UTEST_ASSERT(s1.get_native() != NULL);
        printf("s1 = %s\n", s1.get_native());

        UTEST_ASSERT(s3.get_native() != NULL);
        printf("s3 = %s\n", s3.get_native());
        UTEST_ASSERT(s3.index_of(&s1) == 8);

        s4.truncate(); // ""
        UTEST_ASSERT(s4.get_native() != NULL);
        printf("s4 = %s\n", s4.get_native());
        UTEST_ASSERT(s4.is_empty());
        UTEST_ASSERT(s4.capacity() == s4.length());

        // Search and insert
        UTEST_ASSERT(s1.set_ascii("ABAABBAAABBBAAAABBBB")); // "ABAABBAAABBBAAAABBBB"
        UTEST_ASSERT(s1.get_native() != NULL);
        printf("s1 = %s\n", s1.get_native());

        UTEST_ASSERT(s2.set_ascii("CCCC")); // "CCCC"
        UTEST_ASSERT(s2.get_native() != NULL);
        printf("s2 = %s\n", s2.get_native());

        UTEST_ASSERT(s3.set_ascii("CC")); // "CC"
        UTEST_ASSERT(s3.get_native() != NULL);
        printf("s3 = %s\n", s3.get_native());

        UTEST_ASSERT(s4.set_ascii("abaaccccbbccccaaabbbaaaabbbb")); // "abaaccccbbccccaaabbbaaaabbbb"
        UTEST_ASSERT(s4.get_native() != NULL);
        printf("s4 = %s\n", s4.get_native());

        UTEST_ASSERT(s1.insert(4, &s2)); // ABAACCCCBBAAABBBAAAABBBB
        UTEST_ASSERT(s1.get_native() != NULL);
        printf("s1 = %s\n", s1.get_native());

        UTEST_ASSERT(s1.insert(10, &s2)); // ABAACCCCBBCCCCAAABBBAAAABBBB
        UTEST_ASSERT(s1.get_native() != NULL);
        printf("s1 = %s\n", s1.get_native());
        UTEST_ASSERT(s1.equals_nocase(&s4));

        UTEST_ASSERT(s1.index_of(&s3) == 4);
        UTEST_ASSERT(s1.index_of(5, &s3) == 5);
        UTEST_ASSERT(s1.index_of(6, &s3) == 6);
        UTEST_ASSERT(s1.index_of(7, &s3) == 10);
        UTEST_ASSERT(s1.index_of(-21, &s3) == 10);
        UTEST_ASSERT(s1.rindex_of(&s3) == 12);
        UTEST_ASSERT(s1.rindex_of(-17, &s3) == 6);

        UTEST_ASSERT(s5.set_ascii("BBBBCCCC"));
        UTEST_ASSERT(s5.get_native() != NULL);
        printf("s5 = %s\n", s5.get_native());

        UTEST_ASSERT(s4.set_ascii("CCABAACCCCBB"));
        UTEST_ASSERT(s4.get_native() != NULL);
        printf("s4 = %s\n", s4.get_native());

        UTEST_ASSERT(s1.append(&s2));
        UTEST_ASSERT(s1.get_native() != NULL);
        printf("s1 = %s\n", s1.get_native());

        UTEST_ASSERT(s1.prepend(&s3));
        UTEST_ASSERT(s1.get_native() != NULL);
        printf("s1 = %s\n", s1.get_native());

        UTEST_ASSERT(s1.starts_with(&s4));
        UTEST_ASSERT(s1.ends_with(&s5));
    }

    void test_start_end()
    {
        LSPString s;
        UTEST_ASSERT(s.set_ascii("abcdefghij"));

        // starts_with
        UTEST_ASSERT(s.starts_with('a'));
        UTEST_ASSERT(!s.starts_with('b'));
        UTEST_ASSERT(s.starts_with('e', 4));
        UTEST_ASSERT(!s.starts_with('d', 4));
        UTEST_ASSERT(!s.starts_with('f', 4));

        // starts_with_nocase
        UTEST_ASSERT(s.starts_with_nocase('A'));
        UTEST_ASSERT(!s.starts_with_nocase('B'));
        UTEST_ASSERT(s.starts_with_nocase('E', 4));
        UTEST_ASSERT(!s.starts_with_nocase('D', 4));
        UTEST_ASSERT(!s.starts_with_nocase('F', 4));

        // ends_with
        UTEST_ASSERT(s.ends_with('j'));
        UTEST_ASSERT(!s.ends_with('i'));

        // ends_with_nocase
        UTEST_ASSERT(s.ends_with_nocase('J'));
        UTEST_ASSERT(!s.ends_with_nocase('I'));

        // starts_with_ascii
        UTEST_ASSERT(s.starts_with_ascii(""));
        UTEST_ASSERT(s.starts_with_ascii("a"));
        UTEST_ASSERT(s.starts_with_ascii("ab"));
        UTEST_ASSERT(!s.starts_with_ascii("b"));
        UTEST_ASSERT(s.starts_with_ascii("b", 1));
        UTEST_ASSERT(s.starts_with_ascii("bc", 1));
        UTEST_ASSERT(!s.starts_with_ascii("c", 1));
        UTEST_ASSERT(s.starts_with_ascii("", 10));
        UTEST_ASSERT(!s.starts_with_ascii("j", 10));
        UTEST_ASSERT(!s.starts_with_ascii("", 11));
        UTEST_ASSERT(s.starts_with_ascii("abcdefghij"));
        UTEST_ASSERT(!s.starts_with_ascii("abcdefghijk"));

        // starts_with_ascii_nocase
        UTEST_ASSERT(s.starts_with_ascii_nocase(""));
        UTEST_ASSERT(s.starts_with_ascii_nocase("A"));
        UTEST_ASSERT(s.starts_with_ascii_nocase("AB"));
        UTEST_ASSERT(!s.starts_with_ascii_nocase("B"));
        UTEST_ASSERT(s.starts_with_ascii_nocase("B", 1));
        UTEST_ASSERT(s.starts_with_ascii_nocase("BC", 1));
        UTEST_ASSERT(!s.starts_with_ascii_nocase("C", 1));
        UTEST_ASSERT(s.starts_with_ascii_nocase("", 10));
        UTEST_ASSERT(!s.starts_with_ascii_nocase("J", 10));
        UTEST_ASSERT(!s.starts_with_ascii_nocase("", 11));

        // ends_with_ascii
        UTEST_ASSERT(s.ends_with_ascii(""));
        UTEST_ASSERT(s.ends_with_ascii("j"));
        UTEST_ASSERT(!s.ends_with_ascii("i"));
        UTEST_ASSERT(s.ends_with_ascii("ij"));
        UTEST_ASSERT(s.ends_with_ascii("abcdefghij"));
        UTEST_ASSERT(!s.ends_with_ascii("zabcdefghij"));

        // ends_with_ascii_nocase
        UTEST_ASSERT(s.ends_with_ascii_nocase(""));
        UTEST_ASSERT(s.ends_with_ascii_nocase("J"));
        UTEST_ASSERT(!s.ends_with_ascii_nocase("I"));
        UTEST_ASSERT(s.ends_with_ascii_nocase("IJ"));
        UTEST_ASSERT(s.ends_with_ascii_nocase("ABCDEFGHIJ"));
        UTEST_ASSERT(!s.ends_with_ascii_nocase("ZABCDEFGHIJ"));
    }

    void test_base_hashing()
    {
        printf("Performing hashing test...\n");

        size_t h;
        LSPString s;
        h = s.hash();
        UTEST_ASSERT(h == 0);

        UTEST_ASSERT(s.set_ascii("test"));
        h = s.hash();
        UTEST_ASSERT(h != 0);

        UTEST_ASSERT(s.append('1'));
        UTEST_ASSERT(h != s.hash());
        h = s.hash();

        UTEST_ASSERT(s.prepend('A'));
        UTEST_ASSERT(h != s.hash());
        h = s.hash();

        UTEST_ASSERT(s.prepend_ascii("__"));
        UTEST_ASSERT(h != s.hash());
        h = s.hash();

        UTEST_ASSERT(s.append_ascii("__"));
        UTEST_ASSERT(h != s.hash());
        h = s.hash();

        UTEST_ASSERT(s.set_length(0) == 0);
        UTEST_ASSERT(h != s.hash());
        h = s.hash();
        UTEST_ASSERT(h == 0);
    }

    void test_hash_key()
    {
        lltl::pphash<LSPString, LSPString> h;

        printf("Testing lltl::pphash support...\n");

        // Put values
        for (size_t i=0; i<10; ++i)
        {
            LSPString *v = new LSPString();
            UTEST_ASSERT(v != NULL);
            UTEST_ASSERT(v->fmt_ascii("String %d", int(i)) >= 0);
            UTEST_ASSERT(h.put(v, v, NULL));
        }
        UTEST_ASSERT(h.size() == 10);

        // Fetch values
        for (size_t i=0; i<10; ++i)
        {
            LSPString tmp;
            UTEST_ASSERT(tmp.fmt_ascii("String %d", int(i)) >= 0);
            LSPString **s = h.wbget(&tmp);
            UTEST_ASSERT(s != NULL);
            UTEST_ASSERT(*s != NULL);
            UTEST_ASSERT((*s)->equals(&tmp));
            delete *s;
            *s = NULL;
        }
        UTEST_ASSERT(h.size() == 10);
    }

    void test_line_convert()
    {
        typedef struct cv_t {
            const char *src;
            const char *dos;
            const char *unx;
        } cv_t;

        static const cv_t cases[] =
        {
            { "", "", "" },
            { "test", "test", "test" },
            { "some line", "some line", "some line" },

            { "\n", "\r\n", "\n" },
            { "\r", "\r", "\r" },
            { "\n\n", "\r\n\r\n", "\n\n" },
            { "\r\n", "\r\n", "\n" },
            { "\n\r\n", "\r\n\r\n", "\n\n" },
            { "\n\r\n\r", "\r\n\r\n\r", "\n\n\r" },
            { "\n\r\r\n\r\r", "\r\n\r\r\n\r\r", "\n\r\n\r\r" },

            { "Two\nlines", "Two\r\nlines", "Two\nlines" },
            { "Two\r\nlines", "Two\r\nlines", "Two\nlines" },
            { "Three\nlines\n", "Three\r\nlines\r\n", "Three\nlines\n" },
            { "Three\r\nlines\r\n", "Three\r\nlines\r\n", "Three\nlines\n" },
            { "Three\r\nlines\n", "Three\r\nlines\r\n", "Three\nlines\n" },
            { "Three\nlines\r\n", "Three\r\nlines\r\n", "Three\nlines\n" },

            { "\nYet\nAnother\nTest\nCase\n", "\r\nYet\r\nAnother\r\nTest\r\nCase\r\n", "\nYet\nAnother\nTest\nCase\n" },
            { "\r\nYet\nAnother\nTest\nCase\n", "\r\nYet\r\nAnother\r\nTest\r\nCase\r\n", "\nYet\nAnother\nTest\nCase\n" },
            { "\n\rYet\nAnother\nTest\nCase\r", "\r\n\rYet\r\nAnother\r\nTest\r\nCase\r", "\n\rYet\nAnother\nTest\nCase\r" },
        };

        printf("Testing dos/unix line conversion\n");

        for (size_t i=0, n=sizeof(cases)/sizeof(cv_t); i<n; ++i)
        {
            LSPString a, b;
            printf("Testing string #%d: %s\n", int(i), cases[i].src);
            UTEST_ASSERT(a.set_ascii(cases[i].src));
            UTEST_ASSERT(b.set_ascii(cases[i].src));

            UTEST_ASSERT(a.to_dos());
            UTEST_ASSERT(a.equals_ascii(cases[i].dos));

            UTEST_ASSERT(b.to_unix());
            UTEST_ASSERT(b.equals_ascii(cases[i].unx));
        }
    }

    UTEST_MAIN
    {
        test_basic();
        test_start_end();
        test_base_hashing();
        test_hash_key();
        test_line_convert();
    }
UTEST_END;




