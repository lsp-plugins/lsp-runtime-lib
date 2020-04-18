/*
 * sample.cpp
 *
 *  Created on: 18 апр. 2020 г.
 *      Author: sadko
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

    // u24 constants
    #ifdef ARCH_BE
        static const uint8_t u24s[]             = { 0xff, 0xff, 0xff,  0xbf, 0xff, 0xff,  0x80, 0x00, 0x00,  0x3f, 0xff, 0xff,  0x00, 0x00, 0x01 };
        static const uint8_t s24s[]             = { 0x7f, 0xff, 0xff,  0x3f, 0xff, 0xff,  0x00, 0x00, 0x00,  0xbf, 0xff, 0xff,  0x80, 0x00, 0x01 };
    #else /* ARCH_LE */
        static const uint8_t u24s[]             = { 0xff, 0xff, 0xff,  0xff, 0xff, 0xbf,  0x00, 0x00, 0x80,  0xff, 0xff, 0x3f,  0x01, 0x00, 0x00 };
        static const uint8_t s24s[]             = { 0xff, 0xff, 0x7f,  0xff, 0xff, 0x3f,  0x00, 0x00, 0x00,  0xff, 0xff, 0xbf,  0x01, 0x00, 0x80 };
    #endif

    // u32 constants
    static const uint32_t u32s[]                = { 0xffffffff, 0xbfffffff, 0x80000000, 0x3fffffff, 0x00000001 };
    static const uint32_t s32s[]                = { 0x7fffffff, 0x3fffffff, 0x00000000, 0xbfffffff, 0x80000001 };

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

    void test_to_u8()
    {
        #define CVT(msg, exp, src, from) \
            test_cvt(msg " -> u8", exp, sizeof(exp), src, sizeof(src), mm::SFMT_U8_CPU, mm::from);

        CVT("u8 ",  u8s,    u8s,     SFMT_U8_CPU);
        CVT("u16",  u8t,    u16s,    SFMT_U16_CPU);
        CVT("u24",  u8t,    u24s,    SFMT_U24_CPU);
        CVT("u32",  u8t,    u32s,    SFMT_U32_CPU);

        CVT("s8 ",  u8s,    s8s,     SFMT_S8_CPU);
        CVT("s16",  u8t,    s16s,    SFMT_S16_CPU);
        CVT("s24",  u8t,    s24s,    SFMT_S24_CPU);
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
        CVT("u24",  s8t,    u24s,    SFMT_U24_CPU);
        CVT("u32",  s8t,    u32s,    SFMT_U32_CPU);

        CVT("s8 ",  s8s,    s8s,     SFMT_S8_CPU);
        CVT("s16",  s8t,    s16s,    SFMT_S16_CPU);
        CVT("s24",  s8t,    s24s,    SFMT_S24_CPU);
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
        CVT("u24",  u16t,   u24s,    SFMT_U24_CPU);
        CVT("u32",  u16t,   u32s,    SFMT_U32_CPU);

        CVT("s8 ",  u16e8,  s8s,     SFMT_S8_CPU);
        CVT("s16",  u16s,   s16s,    SFMT_S16_CPU);
        CVT("s24",  u16t,   s24s,    SFMT_S24_CPU);
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
        CVT("u24",  s16t,   u24s,    SFMT_U24_CPU);
        CVT("u32",  s16t,   u32s,    SFMT_U32_CPU);

        CVT("s8 ",  s16e8,  s8s,     SFMT_S8_CPU);
        CVT("s16",  s16s,   s16s,    SFMT_S16_CPU);
        CVT("s24",  s16t,   s24s,    SFMT_S24_CPU);
        CVT("s32",  s16t,   s32s,    SFMT_S32_CPU);

        CVT("f32",  s16f,   f32s,    SFMT_F32_CPU);
        CVT("f64",  s16f,   f64s,    SFMT_F64_CPU);

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
    }
UTEST_END;


