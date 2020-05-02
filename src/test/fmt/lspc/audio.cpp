/*
 * lspc_audio.cpp
 *
 *  Created on: 8 нояб. 2018 г.
 *      Author: sadko
 */

#include <lsp-plug.in/test-fw/utest.h>
#include <lsp-plug.in/test-fw/FloatBuffer.h>
#include <lsp-plug.in/runtime/LSPString.h>
#include <lsp-plug.in/fmt/lspc/lspc.h>
#include <lsp-plug.in/fmt/lspc/AudioWriter.h>
#include <lsp-plug.in/fmt/lspc/AudioReader.h>
#include <lsp-plug.in/lltl/parray.h>

#include <stdlib.h>

#define TOTAL_FRAMES        0xfffff
#define CHANNELS            5
#define INVALID_VALUE       12.34f
#define BLK_SIZE            0x10000

using namespace lsp;

static const float cvalues[CHANNELS] = { -1.0f, -0.5f, 0.0f, 0.5f, 1.0f };

static const size_t formats[] =
{
    LSPC_SAMPLE_FMT_U8LE,
    LSPC_SAMPLE_FMT_U8BE,
    LSPC_SAMPLE_FMT_S8LE,
    LSPC_SAMPLE_FMT_S8BE,
    LSPC_SAMPLE_FMT_U16LE,
    LSPC_SAMPLE_FMT_U16BE,
    LSPC_SAMPLE_FMT_S16LE,
    LSPC_SAMPLE_FMT_S16BE,
    LSPC_SAMPLE_FMT_U24LE,
    LSPC_SAMPLE_FMT_U24BE,
    LSPC_SAMPLE_FMT_S24LE,
    LSPC_SAMPLE_FMT_S24BE,
    LSPC_SAMPLE_FMT_U32LE,
    LSPC_SAMPLE_FMT_U32BE,
    LSPC_SAMPLE_FMT_S32LE,
    LSPC_SAMPLE_FMT_S32BE,
    LSPC_SAMPLE_FMT_F32LE,
    LSPC_SAMPLE_FMT_F32BE,
    LSPC_SAMPLE_FMT_F64LE,
    LSPC_SAMPLE_FMT_F64BE
};

UTEST_BEGIN("runtime.fmt.lspc", audio)

    UTEST_TIMELIMIT(300)

    void create_lspc_file(lltl::parray<FloatBuffer> &v, size_t fmt)
    {
        LSPString file_name;
        lspc::File fd;
        lspc::AudioWriter aw;
        lspc::audio_parameters_t p;

        // Create LSPC file
        io::Path path;
        UTEST_ASSERT(path.fmt("%s/utest-%s.lspc", tempdir(), full_name()));
        status_t res    = fd.create(&path);
        UTEST_ASSERT(res == STATUS_OK);

        // Write audio chunk
        p.channels          = v.size();
        p.sample_format     = fmt;
        p.sample_rate       = 48000;
        p.codec             = LSPC_CODEC_PCM;
        p.frames            = TOTAL_FRAMES;

        res = aw.open(&fd, &p);
        UTEST_ASSERT(res == STATUS_OK);

        // Initialize channel pointers
        const float **vp = reinterpret_cast<const float **>(alloca(sizeof(float *) * v.size()));
        for (size_t i=0, n=v.size(); i<n; ++i)
        {
            FloatBuffer *fb = v.get(i);
            UTEST_ASSERT(fb != NULL);
            vp[i] = fb->data();
        }

        // Write channel data
        size_t written  = 0;
        while (written < TOTAL_FRAMES)
        {
            size_t to_write = TOTAL_FRAMES - written;
            if (to_write > TOTAL_FRAMES)
                to_write = TOTAL_FRAMES;
            res = aw.write_samples(vp, to_write);

            UTEST_ASSERT(res == STATUS_OK);

            written += to_write;
            for (size_t i=0, n=v.size(); i<n; ++i)
                vp[i]  += to_write;
        }

        // Check final status
        res = aw.close();
        UTEST_ASSERT(res == STATUS_OK);
        res = fd.close();
        UTEST_ASSERT(res == STATUS_OK);
    }

    void parse_lspc_file(lltl::parray<FloatBuffer> &v, size_t fmt)
    {
        LSPString file_name;
        lspc::File fd;
        lspc::AudioReader ar;
        lspc::audio_parameters_t p;

        // Create LSPC file
        io::Path path;
        UTEST_ASSERT(path.fmt("%s/utest-%s.lspc", tempdir(), full_name()));
        status_t res    = fd.open(&path);
        UTEST_ASSERT(res == STATUS_OK);

        // Read audio chunk
        res = ar.open(&fd);
        UTEST_ASSERT(res == STATUS_OK);

        res = ar.get_parameters(&p);
        UTEST_ASSERT(res == STATUS_OK);

        UTEST_ASSERT(p.channels == v.size());
        UTEST_ASSERT(p.sample_format == fmt);
        UTEST_ASSERT(p.sample_rate == 48000);
        UTEST_ASSERT(p.codec == LSPC_CODEC_PCM);
        UTEST_ASSERT(p.frames == TOTAL_FRAMES)

        // Initialize channel pointers
        float **vp = reinterpret_cast<float **>(alloca(sizeof(float *) * v.size()));
        for (size_t i=0, n=v.size(); i<n; ++i)
        {
            FloatBuffer *fb = v.get(i);
            UTEST_ASSERT(fb != NULL);

            float *dst = fb->data();
            for (size_t j=0; j<fb->size(); ++j)
                dst[j] = INVALID_VALUE;
            UTEST_ASSERT(fb->valid());

            vp[i] = fb->data();
        }

        // Write channel data
        size_t read  = 0;
        while (read < TOTAL_FRAMES)
        {
            size_t to_read = TOTAL_FRAMES - read;
            if (to_read > BLK_SIZE)
                to_read = BLK_SIZE;

            ssize_t n_read = ar.read_samples(vp, to_read);
            UTEST_ASSERT_MSG(n_read > 0, "Returned invalid value on read: %d, requested=%d", int(n_read), int(to_read));

            read += n_read;
            for (size_t i=0, n=v.size(); i<n; ++i)
                vp[i]  += n_read;
        }

        // Check final status
        res = ar.close();
        UTEST_ASSERT(res == STATUS_OK);
        res = fd.close();
        UTEST_ASSERT(res == STATUS_OK);
    }

    void validate_contents(lltl::parray<FloatBuffer> &src, lltl::parray<FloatBuffer> &dst)
    {
        UTEST_ASSERT(src.size() == dst.size());

        for (size_t i=0, n=src.size(); i<n; ++i)
        {
            FloatBuffer *s = src.get(i), *d = dst.get(i);
            UTEST_ASSERT(s != NULL);
            UTEST_ASSERT(d != NULL);
            UTEST_ASSERT(s->valid());
            UTEST_ASSERT(d->valid());
            if (!s->equals_relative(*d, 1e-2))
            {
                int diff = s->last_diff();
                UTEST_FAIL_MSG("Buffer data for channel %d differs at sample %d: %.6f vs %.6f",
                        int(i), int(diff), s->get(diff), d->get(diff));
            }
        }
    }

    void add_buffer(lltl::parray<FloatBuffer> &v, float value)
    {
        FloatBuffer *fb = new FloatBuffer(TOTAL_FRAMES);
        UTEST_ASSERT(fb != NULL);
        float *dst = fb->data();
        for (size_t i=0; i<TOTAL_FRAMES; ++i)
            dst[i] = value;
        UTEST_ASSERT(fb->valid());
        UTEST_ASSERT(v.add(fb));
    }

    void drop_buffers(lltl::parray<FloatBuffer> &v)
    {
        for (size_t i=0, n=v.size(); i<n; ++i)
        {
            FloatBuffer *fb = v.get(i);
            UTEST_ASSERT(fb != NULL);
            delete fb;
        }
        v.clear();
        UTEST_ASSERT(v.size() == 0);
    }

    UTEST_MAIN
    {
        // Initialize buffers
        lltl::parray<FloatBuffer> src, dst;
        for (size_t i=0; i<CHANNELS; ++i)
        {
            add_buffer(src, cvalues[i]);
            add_buffer(dst, INVALID_VALUE);
        }

        for (size_t i=0, n = sizeof(formats) / sizeof(size_t); i<n; ++i)
        {
            printf("Testing LSPC audio creation sample_format=%d\n", int(formats[i]));
            create_lspc_file(src, formats[i]);
            parse_lspc_file(dst, formats[i]);
            validate_contents(src, dst);
        }

        // Drop buffers
        drop_buffers(src);
        drop_buffers(dst);
    }

UTEST_END


