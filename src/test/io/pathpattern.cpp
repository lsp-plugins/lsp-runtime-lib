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

                    case CMD_TEXT:
                    {
                        LSPString tmp;
                        tmp.set(&sBuffer, cmd->nStart, cmd->nEnd);
                        pTest->printf("TEXT (\"%s\")\n", tmp.get_utf8());
                        break;
                    }

                    case CMD_WILDCARD:
                    {
                        pTest->printf("WILDCARD ('?' x %d)\n", cmd->nEnd - cmd->nStart);
                        break;
                    }

                    case CMD_ANY:
                    {
                        pTest->printf("ANY ('*')\n");
                        break;
                    }

                    case CMD_SPLIT:
                    {
                        pTest->printf("SPLIT ('/', '\\')\n");
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
            "file.!(c|h)",
            "!?file.ext",
            "?file.ext",
            "``quoted`?``.file",
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
        }
    }

    UTEST_MAIN
    {
        test_parse();
    }

UTEST_END


