/*
 * Copyright (C) 2024 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2024 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-runtime-lib
 * Created on: 21 апр. 2024 г.
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
#include <lsp-plug.in/ipc/SharedMem.h>

using namespace lsp;

UTEST_BEGIN("runtime.ipc", shmem)

    void test_basic_operations()
    {
        constexpr size_t shm_size = 0x10000;
        ipc::SharedMem shm;

        printf("Testing basic operations\n");

        UTEST_ASSERT(shm.open("lsp-test.shm", ipc::SharedMem::SHM_RW | ipc::SharedMem::SHM_CREATE, shm_size) == STATUS_OK);
        UTEST_ASSERT(!shm.mapped());
        UTEST_ASSERT(shm.map_size() == -STATUS_NOT_MAPPED);
        UTEST_ASSERT(shm.map_offset() == -STATUS_NOT_MAPPED);
        UTEST_ASSERT(shm.data() == NULL);

        UTEST_ASSERT(shm.map(0, shm_size) == STATUS_OK);
        UTEST_ASSERT(shm.mapped());
        UTEST_ASSERT(shm.map_size() == shm_size);
        UTEST_ASSERT(shm.map_offset() == 0);
        UTEST_ASSERT(shm.data() != NULL);

        memset(shm.data(), 0x55, shm_size);

        UTEST_ASSERT(shm.close() == STATUS_OK);
    }

    void test_multiple_clients()
    {
        constexpr size_t shm_size = 0x10000;
        ipc::SharedMem shm1, shm2, shm3;

        printf("Testing use of the same segment by more than one client\n");

        UTEST_ASSERT(shm1.open("lsp-test.shm", ipc::SharedMem::SHM_WRITE | ipc::SharedMem::SHM_CREATE, shm_size) == STATUS_OK);
        UTEST_ASSERT(shm1.map(0, shm_size) == STATUS_OK);
        UTEST_ASSERT(shm1.data() != NULL);

        memset(shm1.data(), 0xaa, shm_size);

        UTEST_ASSERT(shm2.open("lsp-test.shm", ipc::SharedMem::SHM_READ, 0) == STATUS_OK);
        UTEST_ASSERT(shm2.map(0, shm_size) == STATUS_OK);
        UTEST_ASSERT(shm2.data() != NULL);

        UTEST_ASSERT(shm3.open("lsp-test.shm", ipc::SharedMem::SHM_READ, 0) == STATUS_OK);
        UTEST_ASSERT(shm3.map(0, shm_size) == STATUS_OK);
        UTEST_ASSERT(shm3.data() != NULL);

        printf("shm1 mapped to %p, shm2 mapped to %p, shm3 mapped to %p\n", shm1.data(), shm2.data(), shm3.data());

        UTEST_ASSERT(memcmp(shm1.data(), shm2.data(), shm_size) == 0);
        UTEST_ASSERT(memcmp(shm1.data(), shm3.data(), shm_size) == 0);

        UTEST_ASSERT(shm1.close() == STATUS_OK);
        UTEST_ASSERT(shm2.close() == STATUS_OK);
        UTEST_ASSERT(shm3.close() == STATUS_OK);
    }

    void test_persistent()
    {
        constexpr size_t shm_size = 0x10000;
        ipc::SharedMem shm1, shm2;

        printf("Testing persistent shared memory storage\n");

        // Open first source
        status_t res = shm1.open("lsp-persistent-test.shm", ipc::SharedMem::SHM_RW | ipc::SharedMem::SHM_CREATE | ipc::SharedMem::SHM_PERSIST, shm_size);
        if (res == STATUS_ALREADY_EXISTS)
            res = shm1.open("lsp-persistent-test.shm", ipc::SharedMem::SHM_RW | ipc::SharedMem::SHM_PERSIST, shm_size);

        UTEST_ASSERT(res == STATUS_OK);
        UTEST_ASSERT(shm1.map(0, shm_size) == STATUS_OK);
        UTEST_ASSERT(shm1.data() != NULL);
        printf("  mapped shm1 to %p\n", shm1.data());

        memset(shm1.data(), 0xaa, shm_size);

        // Open second source
        UTEST_ASSERT(shm2.open("lsp-persistent-test.shm", ipc::SharedMem::SHM_RW | ipc::SharedMem::SHM_PERSIST, shm_size) == STATUS_OK);
        UTEST_ASSERT(shm2.map(0, shm_size) == STATUS_OK);
        UTEST_ASSERT(shm2.data() != NULL);
        printf("  mapped shm2 to %p\n", shm1.data());

        UTEST_ASSERT(memcmp(shm1.data(), shm2.data(), shm_size) == 0);
        memset(shm1.data(), 0x55, shm_size);

        // Close first source and reopen
        UTEST_ASSERT(shm1.close() == STATUS_OK);
        UTEST_ASSERT(shm1.open("lsp-persistent-test.shm", ipc::SharedMem::SHM_RW | ipc::SharedMem::SHM_PERSIST, shm_size) == STATUS_OK);
        UTEST_ASSERT(shm1.map(0, shm_size) == STATUS_OK);
        UTEST_ASSERT(shm1.data() != NULL);
        printf("  mapped shm1 to %p\n", shm1.data());

        UTEST_ASSERT(memcmp(shm1.data(), shm2.data(), shm_size) == 0);

        UTEST_ASSERT(shm2.close() == STATUS_OK);
        UTEST_ASSERT(shm1.close() == STATUS_OK);
    }

    void test_persistent_recreate()
    {
        constexpr size_t shm_size = 0x10000;
        ipc::SharedMem shm1, shm2;

        printf("Testing persistent shared memory storage\n");

        // Open first source
        status_t res = shm1.open("lsp-recreate-test.shm", ipc::SharedMem::SHM_RW | ipc::SharedMem::SHM_CREATE, shm_size);
        if (res == STATUS_ALREADY_EXISTS)
            res = shm1.open("lsp-recreate-test.shm", ipc::SharedMem::SHM_RW, shm_size);

        UTEST_ASSERT(res == STATUS_OK);
        UTEST_ASSERT(shm1.map(0, shm_size) == STATUS_OK);
        UTEST_ASSERT(shm1.data() != NULL);
        printf("  mapped shm1 to %p\n", shm1.data());

        memset(shm1.data(), 0xaa, shm_size);

        // Open second source
        UTEST_ASSERT(shm2.open("lsp-recreate-test.shm", ipc::SharedMem::SHM_RW, shm_size) == STATUS_OK);
        UTEST_ASSERT(shm2.map(0, shm_size) == STATUS_OK);
        UTEST_ASSERT(shm2.data() != NULL);
        printf("  mapped shm2 to %p\n", shm1.data());

        UTEST_ASSERT(memcmp(shm1.data(), shm2.data(), shm_size) == 0);
        memset(shm1.data(), 0x55, shm_size);

        // Close first source and reopen
        UTEST_ASSERT(shm1.close() == STATUS_OK);
        UTEST_ASSERT(shm1.open("lsp-recreate-test.shm", ipc::SharedMem::SHM_RW | ipc::SharedMem::SHM_CREATE, shm_size) == STATUS_OK);
        UTEST_ASSERT(shm1.map(0, shm_size) == STATUS_OK);
        UTEST_ASSERT(shm1.data() != NULL);
        printf("  mapped shm1 to %p\n", shm1.data());

        memset(shm1.data(), 0xc3, shm_size);
        UTEST_ASSERT(memcmp(shm1.data(), shm2.data(), shm_size) != 0);

        UTEST_ASSERT(shm2.close() == STATUS_OK);
        UTEST_ASSERT(shm1.close() == STATUS_OK);
    }

    UTEST_MAIN
    {
//        test_basic_operations();
//        test_multiple_clients();
        test_persistent();
        test_persistent_recreate();
    }
UTEST_END;





