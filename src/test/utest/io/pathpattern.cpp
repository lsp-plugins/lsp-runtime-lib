/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-runtime-lib
 * Created on: 17 окт. 2020 г.
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
#include <lsp-plug.in/common/status.h>
#include <lsp-plug.in/io/PathPattern.h>

UTEST_BEGIN("runtime.io", pathpattern)

    class TestPathPattern: public io::PathPattern
    {
        private:
            test_type_t *pTest;

        public:
            explicit TestPathPattern(test_type_t *test)
            {
                pTest = test;
            }

        protected:
            status_t do_dump(size_t offset, cmd_t *cmd)
            {
                for (size_t i=0; i<offset; ++i)
                    pTest->printf(" ");

                if (cmd->bInverse)
                    pTest->printf("!");

                switch (cmd->nCommand)
                {
                    case CMD_SEQUENCE:
                        pTest->printf("SEQUENCE");
                        if (cmd->sChildren.is_empty())
                            pTest->printf(" (empty)\n");
                        else
                        {
                            pTest->printf("\n");
                            for (size_t j=0, n=cmd->sChildren.size(); j<n; ++j)
                                do_dump(offset+2, cmd->sChildren.uget(j));
                        }
                        break;
                    case CMD_AND:
                        pTest->printf("AND ('&')\n");
                        for (size_t j=0, n=cmd->sChildren.size(); j<n; ++j)
                            do_dump(offset+2, cmd->sChildren.uget(j));
                        break;
                    case CMD_OR:
                        pTest->printf("OR ('|')\n");
                        for (size_t j=0, n=cmd->sChildren.size(); j<n; ++j)
                            do_dump(offset+2, cmd->sChildren.uget(j));
                        break;

                    case CMD_PATTERN:
                    {
                        LSPString tmp;
                        tmp.set(&sMask, cmd->nStart, cmd->nStart + cmd->nLength);
                        pTest->printf("PATTERN (\"%s\") start=%d, length=%d, chars=%d\n",
                                tmp.get_utf8(),
                                int(cmd->nStart), int(cmd->nLength), int(cmd->nChars)
                            );
                        break;
                    }

                    case CMD_ANY:
                    {
                        pTest->printf("ANY ('*'");
                        if (cmd->nChars >= 0)
                        {
                            LSPString tmp;
                            tmp.set(&sMask, cmd->nStart, cmd->nStart + cmd->nLength);
                            pTest->printf(", except=\"%s\"", tmp.get_utf8());
                        }
                        pTest->printf(")\n");
                        break;
                    }

                    case CMD_ANYPATH:
                    {
                        pTest->printf("ANYPATH (\"**/\", \"**\\\")\n");
                        break;
                    }

                    default:
                        return STATUS_CORRUPTED;
                }

                return STATUS_OK;
            }

        public:
            status_t dump()
            {
                return (pRoot != NULL) ? do_dump(0, pRoot) : STATUS_OK;
            }
    };

    void test_parse()
    {
        static const char *pattern[] =
        {
            "",
            "file.ext",
            "!file.ext",
            "!!!file.ext",
            "file().txt",
            "file!().txt",
            "file!(-test).txt",
            "file!(!-test).txt",
            "file.!(c|h)",
            "!?file.ext",
            "?file.ext",
            "``quoted`?``.file",
            "``quoted`?``.file`",
            "file.ext?",
            "file*.ext",
            "file.???",
            "file.*",
            "path/file.ext",
            "path\\file.ext",
            "**/file.ext",
            "**/**/file.ext",
            "path/**/file.ext",
            "**/path/**/file.ext",
            "**/path/**/**/file.ext",
            "*.c|*.h",
            "*.c|*.cc|*.cpp|*.h|*.hpp",
            "*.c&test-*|*.h&test-*",
            "!*.c&!*.h",
            "!(*.c|*.h)&!(test-*)",
            "file**.log",
            "file***.log",
            "file(!test).log",
            "file*(!test).log",
            "file(!test)*.log",
            "file*(!test)*.log",
            "file*?*.log",
            "(*.c|*.h)&test-*",
            "(*.c|*.h)&!(*.cc|*.cpp)",
            NULL
        };

        for (const char * const *p = pattern; *p != NULL; ++p)
        {
            TestPathPattern xp(this);
            printf("Testing pattern \"%s\"...\n", *p);
            UTEST_ASSERT(xp.set(*p) == STATUS_OK);
            UTEST_ASSERT(xp.dump() == STATUS_OK);
            printf("\n");
        }
    }

    typedef struct match_t
    {
        const char *pattern;
        bool full;
        const char *value;
        bool match;
    } match_t;

    void test_match_patterns(const match_t *matches)
    {
        TestPathPattern p(this);

        for (const match_t *m = matches; m->pattern != NULL; ++m)
        {
            size_t flags = 0;
            if (m->full)
                flags          |= io::PathPattern::FULL_PATH;
            printf("Testing match for pattern \"%s\", value=\"%s\", full=%s, match=%s\n",
                    m->pattern, m->value,
                    (m->full) ? "true" : "false",
                    (m->match) ? "true" : "false"
                );

            // Test direct
            UTEST_ASSERT(p.set(m->pattern, flags) == STATUS_OK);
            if (p.test(m->value) != m->match)
            {
                p.dump();
                UTEST_FAIL_MSG("Falied direct match for pattern \"%s\", value=\"%s\", match=%s",
                    m->pattern, m->value, (m->match) ? "true" : "false"
                );
            }

            // Test inverse
            UTEST_ASSERT(p.set(m->pattern, flags | io::PathPattern::INVERSE) == STATUS_OK);
            if (p.test(m->value) == m->match)
            {
                p.dump();
                UTEST_FAIL_MSG("Falied inverse match for pattern \"%s\", value=\"%s\", match=%s",
                    m->pattern, m->value, (m->match) ? "true" : "false"
                );
            }
        }
    }

    void test_match_simple()
    {
        static const match_t matches[] =
        {
            // PATTERN match
            { "test",       false,  "test",                 true            },
            { "test",       false,  "",                     false           },
            { "test",       false,  "test.log",             false           },
            { "!test",      false,  "test",                 false           },
            { "!test",      false,  "",                     true            },
            { "!test",      false,  "test.log",             true            },

            { "`",          false,  "`",                    true            },
            { "`",          false,  "``",                   false           },

            { "`a",         false,  "`",                    false           },
            { "`a",         false,  "`a",                   true            },
            { "``a",        false,  "`",                    false           },
            { "``a",        false,  "`a",                   true            },

            { "`a`",        false,  "`a",                   false           },
            { "`a`",        false,  "`a`",                  true            },
            { "``a`",       false,  "`a",                   false           },
            { "``a`",       false,  "`a`",                  true            },
            { "``a``",      false,  "`a",                   false           },
            { "``a``",      false,  "`a`",                  true            },

            // ANY match
            { "*",          false,  "test.log",             true            },
            { "*",          false,  "",                     true            },
            { "*",          false,  "/",                    true            },
            { "!*",         false,  "/",                    false           },
            { "*",          true,   "/",                    false           },
            { "!*",         true,   "/",                    true            },
            { "!()",        false,  "",                     false           },
            { "!()",        false,  "1",                    true            },
            { "!(test)",    false,  "",                     true            },
            { "!(test)",    false,  "tes",                  true            },
            { "!(test)",    false,  "test",                 false           },
            { "!(test)",    false,  "test.txt",             false           },
            { "!(test)",    false,  "local.test",           false           },
            { "!(test)",    false,  "some-test.log",        false           },

            // ANYPATH match
            { "**/",        false,  "",                     true            },
            { "**/",        true,   "",                     true            },
            { "**/",        false,  "/",                    true            },
            { "**/",        true,   "/",                    true            },
            { "**/",        true,   "//",                   true            },
            { "**/",        true,   "/a",                   true            },
            { "**/",        true,   "/a/b/c",               true            },
            { "**/",        true,   "a/b/c",                true            },
            { "**/",        true,   "a/b/",                 true            },
            { "**/",        true,   "a/b/",                 true            },

            // OR match
            { "a|b|c",      false,  "a",                    true            },
            { "a|b|c",      false,  "b",                    true            },
            { "a|b|c",      false,  "c",                    true            },
            { "a|b|c",      false,  "d",                    false           },
            { "!a|b|c",     false,  "a",                    false           },
            { "!a|b|c",     false,  "b",                    true            },
            { "!a|b|c",     false,  "c",                    true            },
            { "!a|b|c",     false,  "d",                    true            },
            { "a|b|!c",     false,  "a",                    true            },
            { "a|b|!c",     false,  "b",                    true            },
            { "a|b|!c",     false,  "c",                    false           },
            { "a|b|!c",     false,  "d",                    true            },
            { "!(a|b|c)",   false,  "a",                    false           },
            { "!(a|b|c)",   false,  "b",                    false           },
            { "!(a|b|c)",   false,  "c",                    false           },
            { "!(a|b|c)",   false,  "d",                    true            },

            // AND match
            { "!a&!b&!c",   false,  "a",                    false           },
            { "!a&!b&!c",   false,  "b",                    false           },
            { "!a&!b&!c",   false,  "c",                    false           },
            { "!a&!b&!c",   false,  "d",                    true            },
            { "a&!b&!c",    false,  "a",                    true            },
            { "a&!b&!c",    false,  "b",                    false           },
            { "a&!b&!c",    false,  "c",                    false           },
            { "a&!b&!c",    false,  "d",                    false           },
            { "!a&!b&c",    false,  "a",                    false           },
            { "!a&!b&c",    false,  "b",                    false           },
            { "!a&!b&c",    false,  "c",                    true            },
            { "!a&!b&c",    false,  "d",                    false           },
            { "!(a&b&c)",   false,  "a",                    true            },
            { "!(a&b&c)",   false,  "b",                    true            },
            { "!(a&b&c)",   false,  "c",                    true            },
            { "!(a&b&c)",   false,  "d",                    true            },

            { NULL,         false,  NULL,                   false           }
        };

        test_match_patterns(matches);
    }

    void test_match_sequence_only()
    {
        static const match_t matches[] =
        {
            // Prefix test
            { "a*",                 false,  "a",                    true            },
            { "a*",                 false,  "ab",                   true            },
            { "a*",                 false,  "abc",                  true            },

            { "a()b*",              false,  "a",                    false           },
            { "a()b*",              false,  "ab",                   true            },
            { "a()b*",              false,  "ad",                   false           },
            { "a()b*",              false,  "abc",                  true            },

            { "a!(b)",              false,  "a",                    true            },
            { "a!(b)",              false,  "b",                    false           },
            { "a!(b)",              false,  "ab",                   false           },
            { "a!(b)",              false,  "ac",                   true            },
            { "a!(b)",              false,  "acb",                  false           },
            { "a!(b)",              false,  "acd",                  true            },
            { "a!(b)",              true,   "a/b",                  false           },

            { "a()b()c",            false,  "a",                    false           },
            { "a()b()c",            false,  "ab",                   false           },
            { "a()b()c",            false,  "abc",                  true            },
            { "a()b()c",            false,  "abcd",                 false           },

            // Postfix test
            { "*b",                 false,  "b",                    true            },
            { "*b",                 false,  "ab",                   true            },
            { "*b",                 false,  "ba",                   false           },
            { "*b",                 false,  "cab",                  true            },

            { "*b()c",              false,  "c",                    false           },
            { "*b()c",              false,  "bc",                   true            },
            { "*b()c",              false,  "ac",                   false           },
            { "*b()c",              false,  "abc",                  true            },

            { "!(a)b",              false,  "a",                    false           },
            { "!(a)b",              false,  "b",                    true            },
            { "!(a)b",              false,  "ab",                   false           },
            { "!(a)b",              false,  "ac",                   false           },
            { "!(a)b",              false,  "cb",                   true            },
            { "!(a)b",              false,  "dcb",                  true            },
            { "!(a)b",              true,   "a/b",                  false           },

            // Variants test
            { "a(!b)c(!d)e",        false,  "ace",                  true            },
            { "a(!b)c(!d)e",        false,  "abcde",                false           },
            { "a(!b)c(!d)e",        false,  "abcxe",                false           },
            { "a(!b)c(!d)e",        false,  "axcde",                false           },
            { "a(!b)c(!d)e",        false,  "abce",                 false           },
            { "a(!b)c(!d)e",        false,  "acde",                 false           },
            { "a(!b)c(!d)e",        false,  "a12c34e",              true            },

            { "ab*cd*ef",           false,  "abbccddeef",           true            },
            { "a*bcb(!b)d",         false,  "aXYbcbcbXYd",          true            },
            { "a*bcb(!b)ded(!d)f",  false,  "aXYbcbcbXYdededXYf",   true            },
            { "a*bcb(!b)ded(!d)f",  false,  "aXYbcbcbXYdeddedXYdf", false           },

            // Paths
            { "**/ab/cd*",          true,   "x/y/z/ab/cd",          true            },
            { "**/ab/cd*",          true,   "x/y/z/ab/cdef",        true            },
            { "**/ab/cd*",          true,   "x/y/z/ab/cd/ef",       false           },
            { "**/ab/cd*",          true,   "x/y/ab/z/cd",          false           },
            { "**/ab/cd*",          true,   "/ab/x/ab/cd",          true            },
            { "**/ab/**/cd*",       true,   "x/y/z/ab/cd",          true            },
            { "**/ab/**/cd*",       true,   "x/y/ab/z/cd",          true            },
            { "**/ab/**/cd*",       true,   "x/y/ab/z/cdef",        true            },
            { "**/ab/**/cd*",       true,   "x/y/ab/z/cd/ef",       false           },
            { "(!**/)ab/**/cd*",    true,   "ab/x/cd",              true            },
            { "(!**/)ab/**/cd*",    true,   "12ab/x/cd",            true            },
            { "(!**/)ab/**/cd*",    true,   "/ab/x/cd",             false           },

            // Sub-pattern
            { "a(b|c)d",            false,  "a",                    false           },
            { "a(b|c)d",            false,  "d",                    false           },
            { "a(b|c)d",            false,  "ad",                   false           },
            { "a(b|c)d",            false,  "abd",                  true            },
            { "a(b|c)d",            false,  "acd",                  true            },
            { "a(b|c)d",            false,  "abcd",                 false           },

            { "a!(b|c)d",           false,  "a",                    false           },
            { "a!(b|c)d",           false,  "ad",                   true            },
            { "a!(b|c)d",           false,  "abd",                  false           },
            { "a!(b|c)d",           false,  "acd",                  false           },
            { "a!(b|c)d",           false,  "aed",                  true            },
            { "a!(b|c)d",           false,  "a12d",                 true            },
            { "a!(b|c)d",           false,  "abcd",                 true            },

            // Logic expressions
            { "a!(*b*|*c*)d",        false,  "abcd",                 false           },
            { "a!(*b*|*c*)d",        false,  "a123b456d",            false           },
            { "a!(*b*|*c*)d",        false,  "a123c456d",            false           },
            { "a!(*b*|*c*)d",        false,  "a123e456d",            true            },
            { "a(*b*&*c*)d",         false,  "a123x456d",            false           },
            { "a(*b*&*c*)d",         false,  "a123b456d",            false           },
            { "a(*b*&*c*)d",         false,  "a123c456d",            false           },
            { "a(*b*&*c*)d",         false,  "a12bc456d",            true            },

            // Including full path
            { "ab/*cd/*ef",         true,   "ab/cd/ef",             true            },
            { "ab/*cd/*ef",         true,   "ab/1cd/2ef",           true            },
            { "ab*/*cd*/*ef",       true,   "abcd/cdef/ghef",       true            },

            { NULL,                 false,  NULL,                   false           }
        };

        test_match_patterns(matches);
    }

    void test_match_brute()
    {
        static const match_t matches[] =
        {
            // Simple group
            { "*(*b*)*",            false,  "a",                    false           },
            { "*(*b*)*",            false,  "b",                    true            },
            { "*(*b*)*",            false,  "ab",                   true            },
            { "*(*b*)*",            false,  "bb",                   true            },
            { "(!*b*)",             false,  "a",                    true            },
            { "(!*b*)",             false,  "b",                    false           },
            { "(!*b*)",             false,  "ab",                   false           },
            { "(!*b*)",             false,  "bb",                   false           },

            // Multiple matches
            { "*(a*|b*)*(c*|d*)",    false,  "a",                   false           },
            { "*(a*|b*)*(c*|d*)",    false,  "b",                   false           },
            { "*(a*|b*)*(c*|d*)",    false,  "c",                   false           },
            { "*(a*|b*)*(c*|d*)",    false,  "d",                   false           },
            { "*(a*|b*)*(c*|d*)",    false,  "ab",                  false           },
            { "*(a*|b*)*(c*|d*)",    false,  "ac",                  true            },
            { "*(a*|b*)*(c*|d*)",    false,  "ad",                  true            },
            { "*(a*|b*)*(c*|d*)",    false,  "ab",                  false           },
            { "*(a*|b*)*(c*|d*)",    false,  "ba",                  false           },
            { "*(a*|b*)*(c*|d*)",    false,  "bc",                  true            },
            { "*(a*|b*)*(c*|d*)",    false,  "bd",                  true            },
            { "*(a*|b*)*(c*|d*)",    false,  "aXc",                 true            },
            { "*(a*|b*)*(c*|d*)",    false,  "aXd",                 true            },
            { "*(a*|b*)*(c*|d*)",    false,  "aXe",                 false           },
            { "*(a*|b*)*(c*|d*)",    false,  "bXc",                 true            },
            { "*(a*|b*)*(c*|d*)",    false,  "bXd",                 true            },
            { "*(a*|b*)*(c*|d*)",    false,  "eXc",                 false           },
            { "*(a*|b*)*(c*|d*)",    false,  "eXd",                 false           },
            { "*(a*|b*)*(c*|d*)",    false,  "abcd",                true            },
            { "*(a*|b*)*(c*|d*)",    false,  "12a34b56c78d90",      true            },
            { "*(a*|b*)*(c*|d*)",    false,  "12a34b56",            false           },
            { "*(a*|b*)*(c*|d*)",    false,  "12c34d56",            false           },
            { "*(a*|b*)*(c*|d*)",    false,  "12a34c56",            true            },
            { "*(a*|b*)*(c*|d*)",    false,  "12c34a56",            false           },
            { "*(a*|b*)*(c*|d*)",    false,  "12a34d56",            true            },
            { "*(a*|b*)*(c*|d*)",    false,  "12d34a56",            false           },
            { "*(a*|b*)*(c*|d*)",    false,  "12b34c56",            true            },
            { "*(a*|b*)*(c*|d*)",    false,  "12c34b56",            false           },
            { "*(a*|b*)*(c*|d*)",    false,  "12b34d56",            true            },
            { "*(a*|b*)*(c*|d*)",    false,  "12d34b56",            false           },

            // Multiple matches with path
            { "*(a*|b*)*(c*|d*)",    true,  "a/b",                  false           },
            { "*(a*|b*)*(c*|d*)",    true,  "a/c",                  false           },
            { "*(a*|b*)*(c*|d*)",    true,  "a/d",                  false           },
            { "*(a*|b*)*(c*|d*)",    true,  "b/a",                  false           },
            { "*(a*|b*)*(c*|d*)",    true,  "b/c",                  false           },
            { "*(a*|b*)*(c*|d*)",    true,  "b/d",                  false           },
            { "*(a*|b*)*(c*|d*)",    true,  "c/a",                  false           },
            { "*(a*|b*)*(c*|d*)",    true,  "c/b",                  false           },
            { "*(a*|b*)*(c*|d*)",    true,  "c/d",                  false           },
            { "*(a*|b*)*(c*|d*)",    true,  "d/a",                  false           },
            { "*(a*|b*)*(c*|d*)",    true,  "d/b",                  false           },
            { "*(a*|b*)*(c*|d*)",    true,  "d/c",                  false           },

            // Test with path at beginning
            { "**/(a*|b*)*(c*|d*)",  true,  "ac",                   true            },
            { "**/(a*|b*)*(c*|d*)",  true,  "ad",                   true            },
            { "**/(a*|b*)*(c*|d*)",  true,  "bc",                   true            },
            { "**/(a*|b*)*(c*|d*)",  true,  "bd",                   true            },
            { "**/(a*|b*)*(c*|d*)",  true,  "x/ac",                 true            },
            { "**/(a*|b*)*(c*|d*)",  true,  "x/ad",                 true            },
            { "**/(a*|b*)*(c*|d*)",  true,  "x/bc",                 true            },
            { "**/(a*|b*)*(c*|d*)",  true,  "x/bd",                 true            },
            { "**/(a*|b*)*(c*|d*)",  true,  "x/a/c",                false           },
            { "**/(a*|b*)*(c*|d*)",  true,  "x/a/d",                false           },
            { "**/(a*|b*)*(c*|d*)",  true,  "x/b/c",                false           },
            { "**/(a*|b*)*(c*|d*)",  true,  "x/b/d",                false           },
            { "**/(a*|b*)*(c*|d*)",  true,  "x/a/a12c",             true            },
            { "**/(a*|b*)*(c*|d*)",  true,  "x/a/a12d",             true            },
            { "**/(a*|b*)*(c*|d*)",  true,  "x/a/b12c",             true            },
            { "**/(a*|b*)*(c*|d*)",  true,  "x/a/b12d",             true            },
            { "**/(a*|b*)*(c*|d*)",  true,  "x/a/0a12c34",          false           },
            { "**/(a*|b*)*(c*|d*)",  true,  "x/a/0a12d34",          false           },
            { "**/(a*|b*)*(c*|d*)",  true,  "x/a/0b12c34",          false           },
            { "**/(a*|b*)*(c*|d*)",  true,  "x/a/0b12d34",          false           },

            { NULL,                 false,  NULL,                   false           }
        };

        test_match_patterns(matches);
    }

    void test_match_examples()
    {
        static const match_t matches[] =
        {
            // One file type
            { "*.c",                        false,  "main.c",               true            },
            { "*.c",                        false,  "src/main.c",           true            },
            { "!*.c",                       false,  "main.c",               false           },
            { "!*.c",                       false,  "main.o",               true            },

            // Two file types
            { "*.c|*.h",                    false,  "main.c",               true            },
            { "*.c|*.h",                    false,  "main.h",               true            },
            { "*.c|*.h",                    false,  "src/main.c",           true            },
            { "*.c|*.h",                    false,  "include/main.h",       true            },

            // Except two file types
            { "(!*.c)&(!*.h)",              false,  "main.c",               false           },
            { "(!*.c)&(!*.h)",              false,  "main.h",               false           },
            { "(!*.c)&(!*.h)",              false,  "src/main.c",           false           },
            { "(!*.c)&(!*.h)",              false,  "include/main.h",       false           },
            { "(!*.c)&(!*.h)",              true,   "src/main.c",           true            },
            { "(!*.c)&(!*.h)",              true,   "include/main.h",       true            },

            // Additional exceptions
            { "(*.c|*.h)&(!test-*)",        false,  "main.c",               true            },
            { "(*.c|*.h)&(!test-*)",        false,  "main.h",               true            },
            { "(*.c|*.h)&(!test-*)",        false,  "test-main.c",          false           },
            { "(*.c|*.h)&(!test-*)",        false,  "test-main.h",          false           },
            { "(*.c|*.h)&(!test-*)",        false,  "src/main.c",           true            },
            { "(*.c|*.h)&(!test-*)",        false,  "include/main.h",       true            },
            { "(*.c|*.h)&(!test-*)",        false,  "src/test-main.c",      false           },
            { "(*.c|*.h)&(!test-*)",        false,  "include/test-main.h",  false           },
            { "(*.c|*.h)&(!test-*)",        true,   "src/main.c",           false           },
            { "(*.c|*.h)&(!test-*)",        true,   "include/main.h",       false           },
            { "(*.c|*.h)&(!test-*)",        true,   "src/test-main.c",      false           },
            { "(*.c|*.h)&(!test-*)",        true,   "include/test-main.h",  false           },

            // With paths
            { "**/((*.c|*.h)&(!test-*))",   false,  "prj/main.c",               true            },
            { "**/((*.c|*.h)&(!test-*))",   false,  "prj/main.h",               true            },
            { "**/((*.c|*.h)&(!test-*))",   false,  "prj/test-main.c",          false           },
            { "**/((*.c|*.h)&(!test-*))",   false,  "prj/test-main.h",          false           },
            { "**/((*.c|*.h)&(!test-*))",   false,  "prj/src/main.c",           true            },
            { "**/((*.c|*.h)&(!test-*))",   false,  "prj/include/main.h",       true            },
            { "**/((*.c|*.h)&(!test-*))",   false,  "prj/src/test-main.c",      false           },
            { "**/((*.c|*.h)&(!test-*))",   false,  "prj/include/test-main.h",  false           },
            { "**/((*.c|*.h)&(!test-*))",   true,   "prj/src/main.c",           true            },
            { "**/((*.c|*.h)&(!test-*))",   true,   "prj/include/main.h",       true            },
            { "**/((*.c|*.h)&(!test-*))",   true,   "prj/src/test-main.c",      false           },
            { "**/((*.c|*.h)&(!test-*))",   true,   "prj/include/test-main.h",  false           },

            // More complicated condition
            { "(*.c|*.h)&(test-*)|(*.o|*.a)&(!test-*)", true,   "main.c",       false           },
            { "(*.c|*.h)&(test-*)|(*.o|*.a)&(!test-*)", true,   "main.h",       false           },
            { "(*.c|*.h)&(test-*)|(*.o|*.a)&(!test-*)", true,   "test-main.c",  true            },
            { "(*.c|*.h)&(test-*)|(*.o|*.a)&(!test-*)", true,   "test-main.h",  true            },
            { "(*.c|*.h)&(test-*)|(*.o|*.a)&(!test-*)", true,   "main.o",       true            },
            { "(*.c|*.h)&(test-*)|(*.o|*.a)&(!test-*)", true,   "main.a",       true            },
            { "(*.c|*.h)&(test-*)|(*.o|*.a)&(!test-*)", true,   "test-main.o",  false           },
            { "(*.c|*.h)&(test-*)|(*.o|*.a)&(!test-*)", true,   "test-main.a",  false           },

            { NULL,                         false,  NULL,                   false           }
        };

        test_match_patterns(matches);
    }

    UTEST_MAIN
    {
        test_parse();
        test_match_simple();
        test_match_sequence_only();
        test_match_brute();
        test_match_examples();
    }

UTEST_END


