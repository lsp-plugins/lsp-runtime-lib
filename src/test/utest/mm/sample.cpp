/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-runtime-lib
 * Created on: 18 апр. 2020 г.
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
#include <lsp-plug.in/common/endian.h>
#include <lsp-plug.in/test-fw/utest.h>
#include <lsp-plug.in/test-fw/ByteBuffer.h>
#include <lsp-plug.in/mm/sample.h>

namespace lsp
{
    // u8 constants
    static const uint8_t u8s[]                  = { 0xff, 0xbf, 0x80, 0x3f, 0x01 };
    static const uint8_t s8s[]                  = { 0x7f, 0x3f, 0x00, 0xbf, 0x81 };

    static const uint8_t u8t[]                  = { 0xff, 0xbf, 0x80, 0x3f, 0x00 };
    static const uint8_t s8t[]                  = { 0x7f, 0x3f, 0x00, 0xbf, 0x80 };

    static const uint8_t u8f[]                  = { 0xff, 0xbf, 0x80, 0x41, 0x01 }; // TODO: fix scale
    static const uint8_t s8f[]                  = { 0x7f, 0x3f, 0x00, 0xc1, 0x81 }; // TODO: fix scale

    // u16 constants
    static const uint16_t u16s[]                = { 0xffff, 0xbfff, 0x8000, 0x3fff, 0x0001 };
    static const uint16_t s16s[]                = { 0x7fff, 0x3fff, 0x0000, 0xbfff, 0x8001 };

    static const uint16_t u16e8[]               = { 0xff00, 0xbf00, 0x8000, 0x3f00, 0x0100 };
    static const uint16_t s16e8[]               = { 0x7f00, 0x3f00, 0x0000, 0xbf00, 0x8100 };

    static const uint16_t u16t[]                = { 0xffff, 0xbfff, 0x8000, 0x3fff, 0x0000 };
    static const uint16_t s16t[]                = { 0x7fff, 0x3fff, 0x0000, 0xbfff, 0x8000 };

    static const uint16_t u16f[]                = { 0xffff, 0xbfff, 0x8000, 0x4001, 0x0001 }; // TODO: fix scale
    static const uint16_t s16f[]                = { 0x7fff, 0x3fff, 0x0000, 0xc001, 0x8001 }; // TODO: fix scale

    // u24 constants in LE form
    static const uint8_t u24s[]                 = { 0xff, 0xff, 0xff,  0xff, 0xff, 0xbf,  0x00, 0x00, 0x80,  0xff, 0xff, 0x3f,  0x01, 0x00, 0x00 };
    static const uint8_t s24s[]                 = { 0xff, 0xff, 0x7f,  0xff, 0xff, 0x3f,  0x00, 0x00, 0x00,  0xff, 0xff, 0xbf,  0x01, 0x00, 0x80 };

    static const uint8_t u24e16[]               = { 0x00, 0x00, 0xff,  0x00, 0x00, 0xbf,  0x00, 0x00, 0x80,  0x00, 0x00, 0x3f,  0x00, 0x00, 0x01 };
    static const uint8_t s24e16[]               = { 0x00, 0x00, 0x7f,  0x00, 0x00, 0x3f,  0x00, 0x00, 0x00,  0x00, 0x00, 0xbf,  0x00, 0x00, 0x81 };

    static const uint8_t u24e8[]                = { 0x00, 0xff, 0xff,  0x00, 0xff, 0xbf,  0x00, 0x00, 0x80,  0x00, 0xff, 0x3f,  0x00, 0x01, 0x00 };
    static const uint8_t s24e8[]                = { 0x00, 0xff, 0x7f,  0x00, 0xff, 0x3f,  0x00, 0x00, 0x00,  0x00, 0xff, 0xbf,  0x00, 0x01, 0x80 };

    static const uint8_t u24t[]                 = { 0xff, 0xff, 0xff,  0xff, 0xff, 0xbf,  0x00, 0x00, 0x80,  0xff, 0xff, 0x3f,  0x00, 0x00, 0x00 };
    static const uint8_t s24t[]                 = { 0xff, 0xff, 0x7f,  0xff, 0xff, 0x3f,  0x00, 0x00, 0x00,  0xff, 0xff, 0xbf,  0x00, 0x00, 0x80 };

    static const uint8_t u24f[]                 = { 0xff, 0xff, 0xff,  0xff, 0xff, 0xbf,  0x00, 0x00, 0x80,  0x01, 0x00, 0x40,  0x01, 0x00, 0x00 }; // TODO: fix scale
    static const uint8_t s24f[]                 = { 0xff, 0xff, 0x7f,  0xff, 0xff, 0x3f,  0x00, 0x00, 0x00,  0x01, 0x00, 0xc0,  0x01, 0x00, 0x80 }; // TODO: fix scale

    // u32 constants
    static const uint32_t u32s[]                = { 0xffffffff, 0xbfffffff, 0x80000000, 0x3fffffff, 0x00000001 };
    static const uint32_t s32s[]                = { 0x7fffffff, 0x3fffffff, 0x00000000, 0xbfffffff, 0x80000001 };

    static const uint32_t u32e8[]               = { 0xffffff00, 0xbfffff00, 0x80000000, 0x3fffff00, 0x00000100 };
    static const uint32_t s32e8[]               = { 0x7fffff00, 0x3fffff00, 0x00000000, 0xbfffff00, 0x80000100 };

    static const uint32_t u32e16[]              = { 0xffff0000, 0xbfff0000, 0x80000000, 0x3fff0000, 0x00010000 };
    static const uint32_t s32e16[]              = { 0x7fff0000, 0x3fff0000, 0x00000000, 0xbfff0000, 0x80010000 };

    static const uint32_t u32e24[]              = { 0xff000000, 0xbf000000, 0x80000000, 0x3f000000, 0x01000000 };
    static const uint32_t s32e24[]              = { 0x7f000000, 0x3f000000, 0x00000000, 0xbf000000, 0x81000000 };

    static const uint32_t u32f[]                = { 0xffffffff, 0xbfffffff, 0x80000000, 0x40000001, 0x00000001 };
    static const uint32_t s32f[]                = { 0x7fffffff, 0x3fffffff, 0x00000000, 0xc0000001, 0x80000001 };

    // float constants
    static const mm::f32_t f32s[]               = { 1.0f, 0.5f, 0.0f, -0.5f, -1.0f };
    static const mm::f64_t f64s[]               = { 1.0, 0.5, 0.0, -0.5, -1.0 };
}

UTEST_BEGIN("runtime.mm", sample)
    void test_cvt(const char *cvt, const void *dst, size_t nbdst, const void *src, size_t nbsrc, size_t to, size_t from)
    {
        printf("  checking %s conversion...\n", cvt);

        ByteBuffer sb(src, nbsrc);
        ByteBuffer eb(dst, nbdst);
        ByteBuffer db(nbdst);
        db.fill_zero();

        UTEST_ASSERT(mm::convert_samples(db.data<uint8_t>(), sb.data<uint8_t>(), 5, to, from));
        UTEST_ASSERT(sb.valid());
        UTEST_ASSERT(db.valid());

//        sb.dump("sb"); db.dump("db"); eb.dump("eb");
        UTEST_ASSERT(
                ::memcmp(db.data<uint8_t>(), eb.data<uint8_t>(), nbdst) == 0,
                sb.dump("sb"); db.dump("db"); eb.dump("eb")
            );
    }

    void test_cvt_f32(const char *cvt, const void *dst, size_t nbdst, const void *src, size_t nbsrc, size_t to, size_t from, float tol)
    {
        printf("  checking %s conversion...\n", cvt);

        ByteBuffer sb(src, nbsrc);
        ByteBuffer eb(dst, nbdst);
        ByteBuffer db(nbdst);
        db.fill_zero();

        UTEST_ASSERT(mm::convert_samples(db.data<uint8_t>(), sb.data<uint8_t>(), 5, to, from));
        UTEST_ASSERT(sb.valid());
        UTEST_ASSERT(db.valid());

        mm::f32_t *a = db.data<mm::f32_t>();
        mm::f32_t *b = eb.data<mm::f32_t>();

        for (size_t i=0; i<5; ++i)
        {
            UTEST_ASSERT(
                (abs(a[i] - b[i]) <= tol),

                sb.dump("sb");
                printf("db: ");
                for (size_t j=0; j<5; ++j)
                    printf("%e ", a[j]);
                printf("\neb: ");
                for (size_t j=0; j<5; ++j)
                    printf("%e ", b[j]);
                printf("\n");
            );
        }
    }

    void test_cvt_f64(const char *cvt, const void *dst, size_t nbdst, const void *src, size_t nbsrc, size_t to, size_t from, float tol)
    {
        printf("  checking %s conversion...\n", cvt);

        ByteBuffer sb(src, nbsrc);
        ByteBuffer eb(dst, nbdst);
        ByteBuffer db(nbdst);
        db.fill_zero();

        UTEST_ASSERT(mm::convert_samples(db.data<uint8_t>(), sb.data<uint8_t>(), 5, to, from));
        UTEST_ASSERT(sb.valid());
        UTEST_ASSERT(db.valid());

        mm::f64_t *a = db.data<mm::f64_t>();
        mm::f64_t *b = eb.data<mm::f64_t>();

        for (size_t i=0; i<5; ++i)
        {
            UTEST_ASSERT(
                (abs(a[i] - b[i]) <= tol),

                sb.dump("sb");
                printf("db: ");
                for (size_t j=0; j<5; ++j)
                    printf("%e ", a[j]);
                printf("\neb: ");
                for (size_t j=0; j<5; ++j)
                    printf("%e ", b[j]);
                printf("\n");
            );
        }
    }

    void test_to_u8()
    {
        #define CVT(msg, exp, src, from) \
            test_cvt(msg " -> u8", exp, sizeof(exp), src, sizeof(src), mm::SFMT_U8_CPU, mm::from);

        CVT("u8 ",  u8s,    u8s,     SFMT_U8_CPU);
        CVT("u16",  u8t,    u16s,    SFMT_U16_CPU);
        CVT("u24",  u8t,    u24s,    SFMT_U24_LE);
        CVT("u32",  u8t,    u32s,    SFMT_U32_CPU);

        CVT("s8 ",  u8s,    s8s,     SFMT_S8_CPU);
        CVT("s16",  u8t,    s16s,    SFMT_S16_CPU);
        CVT("s24",  u8t,    s24s,    SFMT_S24_LE);
        CVT("s32",  u8t,    s32s,    SFMT_S32_CPU);

        CVT("f32",  u8f,    f32s,    SFMT_F32_CPU);
        CVT("f64",  u8f,    f64s,    SFMT_F64_CPU);

        #undef CVT
    }

    void test_to_s8()
    {
        #define CVT(msg, exp, src, from) \
            test_cvt(msg " -> s8", exp, sizeof(exp), src, sizeof(src), mm::SFMT_S8_CPU, mm::from);

        CVT("u8 ",  s8s,    u8s,     SFMT_U8_CPU);
        CVT("u16",  s8t,    u16s,    SFMT_U16_CPU);
        CVT("u24",  s8t,    u24s,    SFMT_U24_LE);
        CVT("u32",  s8t,    u32s,    SFMT_U32_CPU);

        CVT("s8 ",  s8s,    s8s,     SFMT_S8_CPU);
        CVT("s16",  s8t,    s16s,    SFMT_S16_CPU);
        CVT("s24",  s8t,    s24s,    SFMT_S24_LE);
        CVT("s32",  s8t,    s32s,    SFMT_S32_CPU);

        CVT("f32",  s8f,    f32s,    SFMT_F32_CPU);
        CVT("f64",  s8f,    f64s,    SFMT_F64_CPU);

        #undef CVT
    }

    void test_to_u16()
    {
        #define CVT(msg, exp, src, from) \
            test_cvt(msg " -> u16", exp, sizeof(exp), src, sizeof(src), mm::SFMT_U16_CPU, mm::from);

        CVT("u8 ",  u16e8,  u8s,     SFMT_U8_CPU);
        CVT("u16",  u16s,   u16s,    SFMT_U16_CPU);
        CVT("u24",  u16t,   u24s,    SFMT_U24_LE);
        CVT("u32",  u16t,   u32s,    SFMT_U32_CPU);

        CVT("s8 ",  u16e8,  s8s,     SFMT_S8_CPU);
        CVT("s16",  u16s,   s16s,    SFMT_S16_CPU);
        CVT("s24",  u16t,   s24s,    SFMT_S24_LE);
        CVT("s32",  u16t,   s32s,    SFMT_S32_CPU);

        CVT("f32",  u16f,   f32s,    SFMT_F32_CPU);
        CVT("f64",  u16f,   f64s,    SFMT_F64_CPU);

        #undef CVT
    }

    void test_to_s16()
    {
        #define CVT(msg, exp, src, from) \
            test_cvt(msg " -> s16", exp, sizeof(exp), src, sizeof(src), mm::SFMT_S16_CPU, mm::from);

        CVT("u8 ",  s16e8,  u8s,     SFMT_U8_CPU);
        CVT("u16",  s16s,   u16s,    SFMT_U16_CPU);
        CVT("u24",  s16t,   u24s,    SFMT_U24_LE);
        CVT("u32",  s16t,   u32s,    SFMT_U32_CPU);

        CVT("s8 ",  s16e8,  s8s,     SFMT_S8_CPU);
        CVT("s16",  s16s,   s16s,    SFMT_S16_CPU);
        CVT("s24",  s16t,   s24s,    SFMT_S24_LE);
        CVT("s32",  s16t,   s32s,    SFMT_S32_CPU);

        CVT("f32",  s16f,   f32s,    SFMT_F32_CPU);
        CVT("f64",  s16f,   f64s,    SFMT_F64_CPU);

        #undef CVT
    }

    void test_to_u24()
    {
        #define CVT(msg, exp, src, from) \
            test_cvt(msg " -> u24", exp, sizeof(exp), src, sizeof(src), mm::SFMT_U24_LE, mm::from);

        CVT("u8 ",  u24e16, u8s,     SFMT_U8_CPU);
        CVT("u16",  u24e8,  u16s,    SFMT_U16_CPU);
        CVT("u24",  u24s,   u24s,    SFMT_U24_LE);
        CVT("u32",  u24t,   u32s,    SFMT_U32_CPU);

        CVT("s8 ",  u24e16, s8s,     SFMT_S8_CPU);
        CVT("s16",  u24e8,  s16s,    SFMT_S16_CPU);
        CVT("s24",  u24s,   s24s,    SFMT_S24_LE);
        CVT("s32",  u24t,   s32s,    SFMT_S32_CPU);

        CVT("f32",  u24f,   f32s,    SFMT_F32_CPU);
        CVT("f64",  u24f,   f64s,    SFMT_F64_CPU);

        #undef CVT
    }

    void test_to_s24()
    {
        #define CVT(msg, exp, src, from) \
            test_cvt(msg " -> s24", exp, sizeof(exp), src, sizeof(src), mm::SFMT_S24_LE, mm::from);

        CVT("u8 ",  s24e16, u8s,     SFMT_U8_CPU);
        CVT("u16",  s24e8,  u16s,    SFMT_U16_CPU);
        CVT("u24",  s24s,   u24s,    SFMT_U24_LE);
        CVT("u32",  s24t,   u32s,    SFMT_U32_CPU);

        CVT("s8 ",  s24e16, s8s,     SFMT_S8_CPU);
        CVT("s16",  s24e8,  s16s,    SFMT_S16_CPU);
        CVT("s24",  s24s,   s24s,    SFMT_S24_LE);
        CVT("s32",  s24t,   s32s,    SFMT_S32_CPU);

        CVT("f32",  s24f,   f32s,    SFMT_F32_CPU);
        CVT("f64",  s24f,   f64s,    SFMT_F64_CPU);

        #undef CVT
    }

    void test_to_u32()
    {
        #define CVT(msg, exp, src, from) \
            test_cvt(msg " -> u32", exp, sizeof(exp), src, sizeof(src), mm::SFMT_U32_CPU, mm::from);

        CVT("u8 ",  u32e24, u8s,     SFMT_U8_CPU);
        CVT("u16",  u32e16, u16s,    SFMT_U16_CPU);
        CVT("u24",  u32e8,  u24s,    SFMT_U24_LE);
        CVT("u32",  u32s,   u32s,    SFMT_U32_CPU);

        CVT("s8 ",  u32e24, s8s,     SFMT_S8_CPU);
        CVT("s16",  u32e16, s16s,    SFMT_S16_CPU);
        CVT("s24",  u32e8,  s24s,    SFMT_S24_LE);
        CVT("s32",  u32s,   s32s,    SFMT_S32_CPU);

        CVT("f32",  u32f,   f32s,    SFMT_F32_CPU);
        CVT("f64",  u32f,   f64s,    SFMT_F64_CPU);

        #undef CVT
    }

    void test_to_s32()
    {
        #define CVT(msg, exp, src, from) \
            test_cvt(msg " -> s32", exp, sizeof(exp), src, sizeof(src), mm::SFMT_S32_CPU, mm::from);

        CVT("u8 ",  s32e24, u8s,     SFMT_U8_CPU);
        CVT("u16",  s32e16, u16s,    SFMT_U16_CPU);
        CVT("u24",  s32e8,  u24s,    SFMT_U24_LE);
        CVT("u32",  s32s,   u32s,    SFMT_U32_CPU);

        CVT("s8 ",  s32e24, s8s,     SFMT_S8_CPU);
        CVT("s16",  s32e16, s16s,    SFMT_S16_CPU);
        CVT("s24",  s32e8,  s24s,    SFMT_S24_LE);
        CVT("s32",  s32s,   s32s,    SFMT_S32_CPU);

        CVT("f32",  s32f,   f32s,    SFMT_F32_CPU);
        CVT("f64",  s32f,   f64s,    SFMT_F64_CPU);

        #undef CVT
    }

    void test_to_f32()
    {
        #define CVT(msg, tol, src, from) \
            test_cvt_f32(msg " -> f32", f32s, sizeof(f32s), src, sizeof(src), mm::SFMT_F32_CPU, mm::from, tol);

        CVT("u8 ",  0.04,   u8s,     SFMT_U8_CPU);
        CVT("u16",  1e-4,   u16s,    SFMT_U16_CPU);
        CVT("u24",  2e-7,   u24s,    SFMT_U24_LE);
        CVT("u32",  5e-9,   u32s,    SFMT_U32_CPU);

        CVT("s8 ",  0.04,   s8s,     SFMT_S8_CPU);
        CVT("s16",  1e-4,   s16s,    SFMT_S16_CPU);
        CVT("s24",  2e-7,   s24s,    SFMT_S24_LE);
        CVT("s32",  5e-9,   s32s,    SFMT_S32_CPU);

        CVT("f32",  2e-7,   f32s,    SFMT_F32_CPU);
        CVT("f64",  5e-10,  f64s,    SFMT_F64_CPU);

        #undef CVT
    }

    void test_to_f64()
    {
        #define CVT(msg, tol, src, from) \
            test_cvt_f64(msg " -> f64", f64s, sizeof(f64s), src, sizeof(src), mm::SFMT_F64_CPU, mm::from, tol);

        CVT("u8 ",  0.04,   u8s,     SFMT_U8_CPU);
        CVT("u16",  1e-4,   u16s,    SFMT_U16_CPU);
        CVT("u24",  2e-7,   u24s,    SFMT_U24_LE);
        CVT("u32",  5e-9,   u32s,    SFMT_U32_CPU);

        CVT("s8 ",  0.04,   s8s,     SFMT_S8_CPU);
        CVT("s16",  1e-4,   s16s,    SFMT_S16_CPU);
        CVT("s24",  2e-7,   s24s,    SFMT_S24_LE);
        CVT("s32",  5e-9,   s32s,    SFMT_S32_CPU);

        CVT("f32",  2e-7,   f32s,    SFMT_F32_CPU);
        CVT("f64",  5e-10,  f64s,    SFMT_F64_CPU);

        #undef CVT
    }

    UTEST_MAIN
    {
        #define CALL(func)  \
            printf("Calling %s\n", #func); \
            func();

        CALL(test_to_u8);
        CALL(test_to_s8);
        CALL(test_to_u16);
        CALL(test_to_s16);
        CALL(test_to_u24);
        CALL(test_to_s24);
        CALL(test_to_u32);
        CALL(test_to_s32);
        CALL(test_to_f32);
        CALL(test_to_f64);
    }
UTEST_END;


