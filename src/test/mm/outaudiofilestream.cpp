/*
 * outaudiofilestream.cpp
 *
 *  Created on: 20 апр. 2020 г.
 *      Author: sadko
 */

#include <lsp-plug.in/test-fw/utest.h>
#include <lsp-plug.in/test-fw/ByteBuffer.h>
#include <lsp-plug.in/test-fw/FloatBuffer.h>
#include <lsp-plug.in/mm/InAudioFileStream.h>
#include <lsp-plug.in/mm/OutAudioFileStream.h>
#include <lsp-plug.in/stdlib/math.h>

#define BUF_SAMPLES     127
#define FRAMES          2048

UTEST_BEGIN("runtime.mm", outaudiofilestream)

    void validate_file(const io::Path *path, const float *src, size_t srate, float tol)
    {
        mm::InAudioFileStream is;
        mm::audio_stream_t info;

        printf("Reading PCM audio file %s as floating-point samples\n", path->as_native());

        UTEST_ASSERT(is.open(path) == STATUS_OK);
        UTEST_ASSERT(is.info(&info) == STATUS_OK);
        UTEST_ASSERT(info.srate == srate);
        UTEST_ASSERT(info.channels == 2);
        UTEST_ASSERT(info.frames == FRAMES);

        FloatBuffer buf(BUF_SAMPLES * 2);
        ssize_t off = 0;

        while (true)
        {
            // Check position
            UTEST_ASSERT(is.position() == off);

            // Read frames
            ssize_t read = is.read(buf.data(), BUF_SAMPLES);
            if (read < 0)
            {
                UTEST_ASSERT(read == -STATUS_EOF);
                break;
            }
            UTEST_ASSERT(buf.valid());

            // Check contents
            float *ptr = buf.data();
            for (ssize_t i=0; i<read; ++i, ++off, ptr += 2, src += 2)
            {
                UTEST_ASSERT_MSG(float_equals_absolute(src[0], ptr[0], tol), "Samples for channel 0[%d] differ: exp=%e, act=%e", int(off), src[0], ptr[0]);
                UTEST_ASSERT_MSG(float_equals_absolute(src[1], ptr[1], tol), "Samples for channel 1[%d] differ: exp=%e, act=%e", int(off), src[1], ptr[1]);
            }
        }

        UTEST_ASSERT(is.close() == STATUS_OK);
    }

    void test_write_f32(const char *file, const float *src, size_t codec, size_t srate, float tol)
    {
        io::Path path;
        UTEST_ASSERT(path.fmt("%s/%s-%s", tempdir(), full_name(), file));
        printf("Writing PCM audio file %s as floating-point samples\n", path.as_native());

        mm::OutAudioFileStream os;
        mm::audio_stream_t info;
        info.srate      = srate;
        info.channels   = 2;
        info.frames     = FRAMES;
        info.format     = mm::SFMT_F32;

        UTEST_ASSERT(os.open(&path, &info, codec) == STATUS_OK);

        for (ssize_t off=0; off<FRAMES; off += BUF_SAMPLES)
        {
            // Check position
            UTEST_ASSERT(os.position() == off);
            ByteBuffer buf(&src[off * 2], BUF_SAMPLES * 2 * sizeof(float));
            size_t to_write = ((FRAMES - off) > BUF_SAMPLES) ? BUF_SAMPLES : FRAMES-off;

            // Write frames
            ssize_t written = os.write(buf.data<mm::f32_t>(), to_write);
            UTEST_ASSERT(written >= 0);
            UTEST_ASSERT(buf.valid());
        }

        UTEST_ASSERT(os.close() == STATUS_OK);

        validate_file(&path, src, srate, tol);
    }

    void test_write_s16(const char *file, const float *src, size_t codec, size_t srate, float tol)
    {
        io::Path path;
        UTEST_ASSERT(path.fmt("%s/%s-%s", tempdir(), full_name(), file));
        printf("Writing PCM audio file %s as s16 samples\n", path.as_native());

        mm::OutAudioFileStream os;
        mm::audio_stream_t info;
        info.srate      = srate;
        info.channels   = 2;
        info.frames     = FRAMES;
        info.format     = mm::SFMT_S16;

        UTEST_ASSERT(os.open(&path, &info, codec) == STATUS_OK);

        for (ssize_t off=0; off<FRAMES; off += BUF_SAMPLES)
        {
            // Check position
            UTEST_ASSERT(os.position() == off);
            ByteBuffer buf(BUF_SAMPLES * 2 * sizeof(int16_t));
            size_t to_write = ((FRAMES - off) > BUF_SAMPLES) ? BUF_SAMPLES : FRAMES-off;

            int16_t *dst = buf.data<int16_t>();
            for (size_t i=0, j=off*2; i<to_write*2; ++i, ++j)
            {
                float k = src[j] * 32767.5 + 32768.0;
                dst[i] = int16_t(int32_t(k) - 32768);
            }

            // Write frames
            ssize_t written = os.write(dst, to_write);
            UTEST_ASSERT(written >= 0);
            UTEST_ASSERT(buf.valid());
        }

        UTEST_ASSERT(os.close() == STATUS_OK);

        validate_file(&path, src, srate, tol);
    }

    void test_write_u16(const char *file, const float *src, size_t codec, size_t srate, float tol)
    {
        io::Path path;
        UTEST_ASSERT(path.fmt("%s/%s-%s", tempdir(), full_name(), file));
        printf("Writing PCM audio file %s as s16 samples\n", path.as_native());

        mm::OutAudioFileStream os;
        mm::audio_stream_t info;
        info.srate      = srate;
        info.channels   = 2;
        info.frames     = FRAMES;
        info.format     = mm::SFMT_S16;

        UTEST_ASSERT(os.open(&path, &info, codec) == STATUS_OK);

        for (ssize_t off=0; off<FRAMES; off += BUF_SAMPLES)
        {
            // Check position
            UTEST_ASSERT(os.position() == off);
            ByteBuffer buf(BUF_SAMPLES * 2 * sizeof(uint16_t));
            size_t to_write = ((FRAMES - off) > BUF_SAMPLES) ? BUF_SAMPLES : FRAMES-off;

            uint16_t *dst = buf.data<uint16_t>();
            for (size_t i=0, j=off*2; i<to_write*2; ++i, ++j)
            {
                float k = src[j] * 32767.5 + 32768.0;
                dst[i] = uint16_t(int32_t(k));
            }

            // Write frames
            ssize_t written = os.write(dst, to_write);
            UTEST_ASSERT(written >= 0);
            UTEST_ASSERT(buf.valid());
        }

        UTEST_ASSERT(os.close() == STATUS_OK);

        validate_file(&path, src, srate, tol);
    }

    UTEST_MAIN
    {
        // Generate buffer
        float delta = (8 * M_PI) / FRAMES;
        float buf[2 * FRAMES];
        float *dst = buf;
        for (size_t i=0; i<FRAMES; ++i, dst += 2)
        {
            dst[0] = sinf(i * delta);
            dst[1] = cosf(i * delta);
        }

        // Call tests
//        test_write_f32("pcm-f32.wav", buf, mm::AFMT_WAV | mm::CFMT_PCM, 48000, 1e-5f);
//        test_write_s16("pcm-s16.wav", buf, mm::AFMT_WAV | mm::CFMT_PCM, 48000, 5e-5);
//        test_write_u16("pcm-u16.wav", buf, mm::AFMT_WAV | mm::CFMT_PCM, 48000, 5e-5);

        // Call tests
        test_write_f32("alaw-f32.wav", buf, mm::AFMT_WAV | mm::CFMT_ALAW, 48000, 3e-2);
        test_write_s16("alaw-s16.wav", buf, mm::AFMT_WAV | mm::CFMT_ALAW, 48000, 3e-2);
        test_write_u16("alaw-u16.wav", buf, mm::AFMT_WAV | mm::CFMT_ALAW, 48000, 3e-2);
    }
UTEST_END;
