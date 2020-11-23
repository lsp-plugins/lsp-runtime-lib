/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-runtime-lib
 * Created on: 2 мар. 2020 г.
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
#include <lsp-plug.in/i18n/Dictionary.h>

using namespace lsp;

UTEST_BEGIN("runtime.i18n", dictionary)

    void ck_lookup(i18n::IDictionary *d, const char *name, const char *value)
    {
        LSPString v;
        printf("  lookup %s ...\n", name, value);
        UTEST_ASSERT(d->lookup(name, &v) == STATUS_OK);
        printf("  got %s -> %s...\n", name, v.get_utf8());
        UTEST_ASSERT(v.equals_utf8(value));
    }

    UTEST_MAIN
    {
        i18n::Dictionary d;
        i18n::IDictionary *xd;

        printf("Creating dictionary...\n");
        UTEST_ASSERT(d.init(resources()) == STATUS_OK);

        printf("Testing dictionary lookup...\n");
        UTEST_ASSERT(d.lookup("i18n.k1", &xd) == STATUS_NOT_FOUND);
        UTEST_ASSERT(d.lookup("i18n.valid", &xd) == STATUS_OK);
        ck_lookup(xd, "k1", "v1");
        ck_lookup(xd, "k8.k1.k2", "z2");
        ck_lookup(xd, "k7.a3", "x3");

        printf("Testing parameter access...\n");
        LSPString v;
        UTEST_ASSERT(d.lookup("i18n", &v) == STATUS_NOT_FOUND);
        UTEST_ASSERT(d.lookup("i18n.valid", &v) == STATUS_NOT_FOUND);

        ck_lookup(&d, "i18n.valid.k1", "v1");
        ck_lookup(&d, "i18n.valid.k8.k1.k2", "z2");
        ck_lookup(&d, "i18n.valid.k7.a3", "x3");

        UTEST_ASSERT(d.lookup("x.a", &v) == STATUS_NOT_FOUND);
        UTEST_ASSERT(d.lookup("x.a", &v) == STATUS_NOT_FOUND);
    }

UTEST_END


