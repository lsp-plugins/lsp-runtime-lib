/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-runtime-lib
 * Created on: 7 сент. 2019 г.
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
#include <lsp-plug.in/test-fw/helpers.h>
#include <lsp-plug.in/fmt/RoomEQWizard.h>

using namespace lsp;

UTEST_BEGIN("runtime.fmt", roomeqwizard)

    void check_filter(const room_ew::filter_t *f,
            bool enabled, room_ew::filter_type_t type,
            double fc, double gain, double Q
        )
    {
        printf(
            "Filter %s [%d] Fc %.0f Hz Gain %.1f dB Q %.7f\n",
            (f->enabled) ? "ON" : "OFF", f->filterType,
            double(f->fc), double(f->gain), double(f->Q)
        );
        UTEST_ASSERT(f->enabled == enabled);
        UTEST_ASSERT(f->filterType == type);
        UTEST_ASSERT(float_equals_absolute(f->fc, fc));
        UTEST_ASSERT(float_equals_absolute(f->gain, gain));
        if (Q >= 0.0)
            UTEST_ASSERT(float_equals_adaptive(f->Q, Q, 0.5e-3f));
    }

    void read_file(const char *fname)
    {
        room_ew::config_t *cfg = NULL;

        // Load the equalizer settings
        io::Path path;
        UTEST_ASSERT(path.set(resources()) == STATUS_OK);
        UTEST_ASSERT(path.append_child(fname) == STATUS_OK);
        UTEST_ASSERT(room_ew::load(&path, &cfg) == STATUS_OK);

        // Check configuration
        UTEST_ASSERT(cfg != NULL);
        UTEST_ASSERT(::strcmp(cfg->sNotes, "test notes") == 0);
        UTEST_ASSERT(::strcmp(cfg->sEqType, "Generic") == 0);
        UTEST_ASSERT(cfg->nVerMaj == 5);
        UTEST_ASSERT(cfg->nVerMin == 19);
        UTEST_ASSERT(cfg->nFilters == 20);
        UTEST_ASSERT(cfg->vFilters != NULL);

        room_ew::filter_t *vf = cfg->vFilters;
        size_t idx = 0;

        check_filter(&vf[idx++], true, room_ew::PK, 100.00, 10.00, 0.7100000);
        check_filter(&vf[idx++], true, room_ew::LS, 1000.00, 10.00, -1);
        check_filter(&vf[idx++], true, room_ew::HS, 1000.00, -10.00, -1);
        check_filter(&vf[idx++], true, room_ew::PK, 10000.00, -10.00, 0.7100000);
        check_filter(&vf[idx++], true, room_ew::HS, 321.00, 0.00, -1);
        check_filter(&vf[idx++], true, room_ew::NONE, 100.00, 0.00, -1);
        check_filter(&vf[idx++], true, room_ew::PK, 100.00, 0.00, 10.0000000);
        check_filter(&vf[idx++], true, room_ew::MODAL, 100.00, 0.00, 13.643000);
        check_filter(&vf[idx++], true, room_ew::LP, 100.00, 0.00, 0.7071068);
        check_filter(&vf[idx++], true, room_ew::HP, 100.00, 0.00, 0.7071068);
        check_filter(&vf[idx++], true, room_ew::LPQ, 100.00, 0.00, 0.7070000);
        check_filter(&vf[idx++], true, room_ew::HPQ, 100.00, 0.00, 0.7070000);
        check_filter(&vf[idx++], true, room_ew::LS, 100.00, 0.00, -1);
        check_filter(&vf[idx++], true, room_ew::HS, 100.00, 0.00, -1);
        check_filter(&vf[idx++], true, room_ew::LS6, 100.00, 0.00, -1);
        check_filter(&vf[idx++], true, room_ew::HS6, 100.00, 0.00, -1);
        check_filter(&vf[idx++], true, room_ew::LS12, 100.00, 0.00, -1);
        check_filter(&vf[idx++], true, room_ew::HS12, 100.00, 0.00, -1);
        check_filter(&vf[idx++], false, room_ew::NO, 100.00, 0.00, -1);
        check_filter(&vf[idx++], true, room_ew::AP, 100.00, 0.00, 0.7070000);

        ::free(cfg);
    }

    void read_file2(const char *fname)
    {
        room_ew::config_t *cfg = NULL;

        // Load the equalizer settings
        io::Path path;
        UTEST_ASSERT(path.set(resources()) == STATUS_OK);
        UTEST_ASSERT(path.append_child(fname) == STATUS_OK);
        UTEST_ASSERT(room_ew::load(&path, &cfg) == STATUS_OK);

        // Check configuration
        UTEST_ASSERT(cfg != NULL);
        UTEST_ASSERT(::strcmp(cfg->sNotes, "This file demonstrates all filter types the Generic equalizer supports") == 0);
        UTEST_ASSERT(::strcmp(cfg->sEqType, "Generic") == 0);
        UTEST_ASSERT(cfg->nVerMaj == 5);
        UTEST_ASSERT(cfg->nVerMin == 1);
        UTEST_ASSERT(cfg->nFilters == 20);
        UTEST_ASSERT(cfg->vFilters != NULL);

        room_ew::filter_t *vf = cfg->vFilters;
        size_t idx = 0;

        check_filter(&vf[idx++], true, room_ew::PK, 50.00, -10.00, 2.50);
        check_filter(&vf[idx++], true, room_ew::MODAL, 100.00, 3.0, 5.41);
        check_filter(&vf[idx++], true, room_ew::LP, 8.0, 0.0, -1);
        check_filter(&vf[idx++], true, room_ew::HP, 30.00, 0.0, -1);
        check_filter(&vf[idx++], true, room_ew::LPQ, 10.00, 0.0, 0.4);
        check_filter(&vf[idx++], true, room_ew::HPQ, 20.00, 0.0, 0.5);
        check_filter(&vf[idx++], true, room_ew::LS, 300.00, 5.0, -1);
        check_filter(&vf[idx++], true, room_ew::HS, 1.00, -3.0, -1);
        check_filter(&vf[idx++], true, room_ew::LS12, 2.00, -5.0, -1);
        check_filter(&vf[idx++], true, room_ew::HS12, 500.0, 5.0, -1);
        check_filter(&vf[idx++], true, room_ew::LS6, 50.0, 7.2, -1);
        check_filter(&vf[idx++], true, room_ew::HS6, 12.0, 10.0, -1);
        check_filter(&vf[idx++], true, room_ew::NO, 800.00, 0.00, -1);
        check_filter(&vf[idx++], true, room_ew::AP, 900.00, 0.00, 0.707);

        ::free(cfg);
    }

    void read_file3(const char *fname)
    {
        room_ew::config_t *cfg = NULL;

        // Load the equalizer settings
        io::Path path;
        UTEST_ASSERT(path.set(resources()) == STATUS_OK);
        UTEST_ASSERT(path.append_child(fname) == STATUS_OK);
        UTEST_ASSERT(room_ew::load(&path, &cfg) == STATUS_OK);

        // Check configuration
        UTEST_ASSERT(cfg != NULL);
        UTEST_ASSERT(::strcmp(cfg->sNotes, "") == 0);
        UTEST_ASSERT(::strcmp(cfg->sEqType, "") == 0);
        UTEST_ASSERT(cfg->nVerMaj == 0);
        UTEST_ASSERT(cfg->nVerMin == 0);
        UTEST_ASSERT(float_equals_adaptive(cfg->fPreamp, -6.9));
        UTEST_ASSERT(cfg->nFilters == 10);
        UTEST_ASSERT(cfg->vFilters != NULL);

        room_ew::filter_t *vf = cfg->vFilters;
        size_t idx = 0;

        check_filter(&vf[idx++], true, room_ew::PK, 91.0, -1.7, 1.525);
        check_filter(&vf[idx++], true, room_ew::LSC, 112.0, 2.1, 1.387);
        check_filter(&vf[idx++], true, room_ew::PK, 539.0, 5.4, 0.585);
        check_filter(&vf[idx++], true, room_ew::PK, 1918.0, -4.1, 2.221);
        check_filter(&vf[idx++], true, room_ew::PK, 3103.0, 7.8, 1.502);
        check_filter(&vf[idx++], true, room_ew::PK, 4493.0, -6.6, 3.300);
        check_filter(&vf[idx++], true, room_ew::PK, 6100.0, 4.4, 1.900);
        check_filter(&vf[idx++], true, room_ew::HSC, 8600.0, 2.0, 2.537);
        check_filter(&vf[idx++], true, room_ew::PK, 11200.0, -7.0, 3.001);
        check_filter(&vf[idx++], true, room_ew::PK, 14886.0, 4.2, 2.100);

//        Filter 1: ON PK Fc 91 Hz Gain -1.7 dB Q 1.525
//        Filter 2: ON LSC Fc 112 Hz Gain 2.1 dB Q 1.387
//        Filter 3: ON PK Fc 539 Hz Gain 5.4 dB Q 0.585
//        Filter 4: ON PK Fc 1918 Hz Gain -4.1 dB Q 2.221
//        Filter 5: ON PK Fc 3103 Hz Gain 7.8 dB Q 1.502
//        Filter 6: ON PK Fc 4493 Hz Gain -6.6 dB Q 3.300
//        Filter 7: ON PK Fc 6100 Hz Gain 4.4 dB Q 1.900
//        Filter 8: ON HSC Fc 8600 Hz Gain 2.0 dB Q 2.537
//        Filter 9: ON PK Fc 11200 Hz Gain -7.0 dB Q 3.001
//        Filter 10: ON PK Fc 14886 Hz Gain 4.2 dB Q 2.100

        ::free(cfg);
    }

    UTEST_MAIN
    {
//        printf("Testing binary file...\n");
//        read_file("fmt/rew/Test11.req");
//
//        printf("Testing text file...\n");
//        read_file("fmt/rew/Test11.txt");
//
//        printf("Testing another file...\n");
//        read_file2("fmt/apo/demo.txt");

        printf("Testing another file...\n");
        read_file3("fmt/apo/effects.apo.txt");
    }

UTEST_END


