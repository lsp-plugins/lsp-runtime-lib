/*
 * Copyright (C) 2022 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2022 Vladimir Sadovnikov <sadko4u@gmail.com>
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

#ifdef PLATFORM_WINDOWS
    #define DELAY_PRECISION 6
#else
    #define DELAY_PRECISION 1
#endif /* PLATFORM_WINDOWS */

UTEST_BEGIN("runtime.runtime", system)

    void test_sleep_msec(size_t period)
    {
        system::time_t start, end;
        printf("Testing sleep for %d milliseconds\n", int(period));

        system::get_time(&start);
        UTEST_ASSERT(system::sleep_msec(period) == STATUS_OK);
        system::get_time(&end);

        wssize_t delay_ms = wssize_t(end.seconds - start.seconds) * 1000 + wssize_t(end.nanos - start.nanos) / 1000000;

        printf("Requested delay: %ld, actual delay: %ld\n", long(period), long(delay_ms));

        // The delay should not be less than expected.
        UTEST_ASSERT(delay_ms >= wssize_t(period));
        // The delay should be precise: not more than 1 ms error.
        UTEST_ASSERT_MSG((delay_ms - period) <= DELAY_PRECISION,
            "Expected delay: %ld but actual delay is %ld", long(period), long(delay_ms));
    }

    UTEST_MAIN
    {
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







