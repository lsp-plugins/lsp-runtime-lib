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
#include <lsp-plug.in/i18n/JsonDictionary.h>

using namespace lsp;

UTEST_BEGIN("runtime.i18n", json_dictionary)

    void ck_att(i18n::IDictionary *d, size_t index, const char *name, const char *value)
    {
        LSPString k, v;
        printf("  fetching %s -> %s...\n", name, value);
        UTEST_ASSERT(d->get_value(index, &k, &v) == STATUS_OK);
        printf("  fetched %s -> %s...\n", k.get_utf8(), v.get_utf8());
        UTEST_ASSERT(k.equals_utf8(name));
        UTEST_ASSERT(v.equals_utf8(value));
    }

    void ck_child(i18n::IDictionary *d, size_t index, const char *name, bool special = false)
    {
        LSPString k;
        i18n::IDictionary *c = NULL;

        printf("  fetching child: -> %s...\n", name);
        UTEST_ASSERT(d->get_child(index, &k, &c) == STATUS_OK);
        UTEST_ASSERT(k.equals_utf8(name));

        size_t idx = 0;
        if (special)
            ck_att(c, idx++, "", "special_case");
        ck_att(c, idx++, "a1", "x1");
        ck_att(c, idx++, "a2", "x2");
        ck_att(c, idx++, "a3", "x3");
    }

    void ck_lookup(i18n::IDictionary *d, const char *name, const char *value)
    {
        LSPString v;
        printf("  lookup %s ...\n", name, value);
        UTEST_ASSERT(d->lookup(name, &v) == STATUS_OK);
        printf("  got %s -> %s...\n", name, v.get_utf8());
        UTEST_ASSERT(v.equals_utf8(value));
    }

    void validate(i18n::IDictionary *d)
    {
        ck_att(d, 0, "k1", "v1");
        ck_child(d, 1, "k2", true);
        ck_child(d, 2, "k3");
        ck_child(d, 3, "k4");
        ck_child(d, 4, "k5");
        ck_child(d, 5, "k6");
        ck_child(d, 6, "k7");

        ck_lookup(d, "k1", "v1");
        ck_lookup(d, "k2.a3", "x3");
        ck_lookup(d, "k8.k1.k2", "z2");
        ck_lookup(d, "k2", "special_case");
        ck_lookup(d, "k8.k1", "special_case2");
    }

    UTEST_MAIN
    {
        i18n::JsonDictionary d;
        i18n::IDictionary *xd = NULL;
        io::Path path;

        printf("Testing load of valid dictionary...\n");
        UTEST_ASSERT(path.fmt("%s/i18n/valid.json", resources()) > 0);
        UTEST_ASSERT(d.init(&path) == STATUS_OK);

        printf("Testing lookup of dictionary node...\n");

        UTEST_ASSERT(d.lookup("lalala", &xd) == STATUS_NOT_FOUND);
        UTEST_ASSERT(d.lookup("k2", &xd) == STATUS_OK);
        ck_att(xd, 0, "", "special_case");
        ck_att(xd, 1, "a1", "x1");
        ck_att(xd, 2, "a2", "x2");
        ck_att(xd, 3, "a3", "x3");
        UTEST_ASSERT(d.lookup("k8.k1", &xd) == STATUS_OK);
        ck_att(xd, 0, "", "special_case2");
        ck_att(xd, 1, "k2", "z2");

        printf("Validating dictionary...\n");
        validate(&d);

        printf("Testing load of invalid dictionary...\n");
        UTEST_ASSERT(path.fmt("%s/i18n/invalid1.json", resources()) > 0);
        UTEST_ASSERT(d.init(&path) != STATUS_OK);

        UTEST_ASSERT(path.fmt("%s/i18n/invalid2.json", resources()) > 0);
        UTEST_ASSERT(d.init(&path) != STATUS_OK);

        UTEST_ASSERT(path.fmt("%s/i18n/invalid3.json", resources()) > 0);
        UTEST_ASSERT(d.init(&path) != STATUS_OK);

        printf("Validating that state of dictionary has not changed...\n");
        validate(&d);
    }

UTEST_END
