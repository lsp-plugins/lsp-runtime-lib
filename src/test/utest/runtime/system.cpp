/*
 * Copyright (C) 2024 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2024 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-runtime-lib
 * Created on: 24 июн. 2022 г.
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
#include <lsp-plug.in/runtime/system.h>

#define DELAY_PRECISION 10

UTEST_BEGIN("runtime.runtime", system)

    void test_sleep_msec(size_t period)
    {
        system::time_millis_t start, end;
        printf("Testing sleep for %d milliseconds\n", int(period));

        start = system::get_time_millis();
        UTEST_ASSERT(system::sleep_msec(period) == STATUS_OK);
        end = system::get_time_millis();

        wssize_t delay_ms = end - start;
        printf("Requested delay: %ld, actual delay: %ld\n", long(period), long(delay_ms));

        // The delay should not be less than expected.
        UTEST_ASSERT(delay_ms >= wssize_t(period));
        // The delay should be precise: not more than 1 ms error.
        UTEST_ASSERT_MSG((delay_ms - period) <= DELAY_PRECISION,
            "Expected delay: %ld but actual delay is %ld", long(period), long(delay_ms));
    }

    void test_time_measure()
    {
        system::time_t ctime;
        system::time_millis_t millis, computed;

        millis = system::get_time_millis();
        system::get_time(&ctime);
        computed = system::time_millis_t(ctime.seconds) * 1000 + system::time_millis_t(ctime.nanos) / 1000000;

        printf("ctime    = %lld seconds %lld nanos\n", (long long)(ctime.seconds), (long long)(ctime.nanos));
        printf("millis   = %lld\n", (long long)millis);
        printf("computed = %lld\n", (long long)computed);
        printf("rate     = %lld\n", (long long)(millis/computed));

        UTEST_ASSERT(ctime.nanos < 1000000000u);
        UTEST_ASSERT(computed >= millis);
        UTEST_ASSERT((computed - millis) <= 10);
    }

    void test_volume_info()
    {
        lltl::parray<system::volume_info_t> list;
        UTEST_ASSERT(system::get_volume_info(&list) == STATUS_OK);
        lsp_finally{ system::free_volume_info(&list); };

        printf("List of available volumes:\n");

        for (size_t i=0, n=list.size(); i<n; ++i)
        {
            system::volume_info_t *v = list.uget(i);
            UTEST_ASSERT(v != NULL);

            printf("dev %s root=%s type=%s mounted on %s dummy=%s, remote=%s, drive=%s\n",
                v->device.get_native(),
                v->root.get_native(),
                v->name.get_native(),
                v->target.get_native(),
                (v->flags & system::VF_DUMMY) ? "true" : "false",
                (v->flags & system::VF_REMOTE) ? "true" : "false",
                (v->flags & system::VF_DRIVE) ? "true" : "false");
        }
    }

    void test_get_user_login()
    {
        LSPString login;
        UTEST_ASSERT(system::get_user_login(&login) == STATUS_OK);
        printf("User login is: %s\n", login.get_native());
    }

    UTEST_MAIN
    {
        // Test user login
        test_get_user_login();

        // Test reading of volume info
        test_volume_info();

        // Test time measurement
        test_time_measure();

        // Test the system::sleep_msec function.
        test_sleep_msec(10);
        test_sleep_msec(15);
        test_sleep_msec(100);
        test_sleep_msec(105);
        test_sleep_msec(150);
        test_sleep_msec(1000);
        test_sleep_msec(1005);
    }
UTEST_END;







