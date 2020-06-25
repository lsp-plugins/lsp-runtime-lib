/*
 * dictionary.cpp
 *
 *  Created on: 2 мар. 2020 г.
 *      Author: sadko
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


