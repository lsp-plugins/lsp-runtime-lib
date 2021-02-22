/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-runtime-lib
 * Created on: 26 окт. 2020 г.
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
#include <lsp-plug.in/resource/Environment.h>
#include <lsp-plug.in/stdlib/string.h>

using namespace lsp;

UTEST_BEGIN("runtime.resource", environment)

    UTEST_MAIN
    {
        resource::Environment env, *copy;
        const char *s;

        // Add values
        UTEST_ASSERT(env.set("k1", "value1") == STATUS_OK);
        UTEST_ASSERT(env.set("k2", "value2") == STATUS_OK);
        UTEST_ASSERT(env.set("k1", "value3") == STATUS_OK);

        // Check presence
        UTEST_ASSERT(env.contains("k1"));
        UTEST_ASSERT(env.contains("k2"));
        UTEST_ASSERT(!env.contains("k3"));
        UTEST_ASSERT(!env.contains("k4"));

        // Fetch values
        UTEST_ASSERT((s = env.get_utf8("k1")) != NULL);
        UTEST_ASSERT(strcmp(s, "value3") == 0);
        UTEST_ASSERT((s = env.get_utf8("k2")) != NULL);
        UTEST_ASSERT(strcmp(s, "value2") == 0);
        UTEST_ASSERT((s = env.get_utf8("k3")) == NULL);
        UTEST_ASSERT((s = env.get_utf8("k4", "dfl")) != NULL);
        UTEST_ASSERT(strcmp(s, "dfl") == 0);

        // Clone values
        UTEST_ASSERT((copy = env.clone()) != NULL);

        // Process original environment
        // Remove values
        UTEST_ASSERT(env.remove("k1") == STATUS_OK);
        UTEST_ASSERT(env.remove("k2") == STATUS_OK);
        UTEST_ASSERT(env.remove("k3") == STATUS_NOT_FOUND);
        UTEST_ASSERT(env.remove("k4") == STATUS_NOT_FOUND);

        // Fetch values again
        UTEST_ASSERT((s = env.get_utf8("k1")) == NULL);
        UTEST_ASSERT((s = env.get_utf8("k2")) == NULL);
        UTEST_ASSERT((s = env.get_utf8("k3")) == NULL);
        UTEST_ASSERT((s = env.get_utf8("k4", "dfl")) != NULL);
        UTEST_ASSERT(strcmp(s, "dfl") == 0);

        // Check presence again
        UTEST_ASSERT(!env.contains("k1"));
        UTEST_ASSERT(!env.contains("k2"));
        UTEST_ASSERT(!env.contains("k3"));
        UTEST_ASSERT(!env.contains("k4"));

        // Process cloned environment
        // Remove values
        UTEST_ASSERT(copy->remove("k1") == STATUS_OK);
        UTEST_ASSERT(copy->remove("k2") == STATUS_OK);
        UTEST_ASSERT(copy->remove("k3") == STATUS_NOT_FOUND);
        UTEST_ASSERT(copy->remove("k4") == STATUS_NOT_FOUND);

        // Fetch values again
        UTEST_ASSERT((s = copy->get_utf8("k1")) == NULL);
        UTEST_ASSERT((s = copy->get_utf8("k2")) == NULL);
        UTEST_ASSERT((s = copy->get_utf8("k3")) == NULL);
        UTEST_ASSERT((s = copy->get_utf8("k4", "dfl")) != NULL);
        UTEST_ASSERT(strcmp(s, "dfl") == 0);

        // Check presence again
        UTEST_ASSERT(!copy->contains("k1"));
        UTEST_ASSERT(!copy->contains("k2"));
        UTEST_ASSERT(!copy->contains("k3"));
        UTEST_ASSERT(!copy->contains("k4"));

        // Destroy environment
        delete copy;
    }

UTEST_END



