/*
 * Copyright (C) 2025 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2025 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-runtime-lib
 * Created on: 28 авг. 2018 г.
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
#include <lsp-plug.in/fmt/lspc/File.h>
#include <lsp-plug.in/runtime/LSPString.h>
#include <lsp-plug.in/stdlib/string.h>
#include <lsp-plug.in/test-fw/utest.h>
#include <lsp-plug.in/test-fw/ByteBuffer.h>

#define EXTRA_SIZE          0x10
#define BUFFER_SIZE         0x100000

namespace lsp
{
    typedef struct lspc_chunk_audio_header_v2_t: public lspc::chunk_audio_header_t
    {
        uint8_t extra[EXTRA_SIZE];
    } lspc_chunk_audio_header_v2_t;

    typedef struct lspc_chunk_audio_profile_v2_t: public lspc::chunk_audio_profile_t
    {
        uint8_t extra[EXTRA_SIZE];
    } lspc_chunk_audio_profile_v2_t;
}

UTEST_BEGIN("runtime.fmt.lspc", lspc)

    void create_lspc_file(const ByteBuffer &content, bool v2)
    {
        lspc::File fd;
        io::Path path;
        UTEST_ASSERT(path.fmt("%s/utest-%s.lspc", tempdir(), full_name()));

        printf("Writing file %s ...\n", path.as_native());
        status_t res    = fd.create(&path);

        UTEST_ASSERT(res == STATUS_OK);

        // Write audio chunk
        lspc::ChunkWriter *wr = fd.write_chunk(LSPC_CHUNK_AUDIO);
        UTEST_ASSERT(wr != NULL);

        if (v2)
        {
            lspc_chunk_audio_header_v2_t ahdr2;
            bzero(&ahdr2, sizeof(ahdr2));

            ahdr2.common.version    = 2;
            ahdr2.common.size       = sizeof(lspc_chunk_audio_header_v2_t);
            ahdr2.channels          = 1;
            ahdr2.sample_format     = lspc::SAMPLE_FMT_F32LE;
            ahdr2.sample_rate       = 48000;
            ahdr2.codec             = lspc::CODEC_PCM;
            ahdr2.frames            = (content.size() / sizeof(float));

            ahdr2.channels          = BE_TO_CPU(ahdr2.channels);
            ahdr2.sample_format     = BE_TO_CPU(ahdr2.sample_format);
            ahdr2.sample_rate       = BE_TO_CPU(ahdr2.sample_rate);
            ahdr2.codec             = BE_TO_CPU(ahdr2.codec);
            ahdr2.frames            = BE_TO_CPU(ahdr2.frames);

            for (size_t i=0; i < EXTRA_SIZE; ++i)
                ahdr2.extra[i]          = uint8_t(i + 1);

            res                     = wr->write_header(&ahdr2);
            UTEST_ASSERT(res == STATUS_OK);
        }
        else
        {
            lspc::chunk_audio_header_t ahdr1;
            bzero(&ahdr1, sizeof(ahdr1));

            ahdr1.common.version    = 1;
            ahdr1.common.size       = sizeof(lspc::chunk_audio_header_t);
            ahdr1.channels          = 1;
            ahdr1.sample_format     = lspc::SAMPLE_FMT_F32LE;
            ahdr1.sample_rate       = 48000;
            ahdr1.codec             = lspc::CODEC_PCM;
            ahdr1.frames            = (content.size() / sizeof(float));

            ahdr1.channels          = BE_TO_CPU(ahdr1.channels);
            ahdr1.sample_format     = BE_TO_CPU(ahdr1.sample_format);
            ahdr1.sample_rate       = BE_TO_CPU(ahdr1.sample_rate);
            ahdr1.codec             = BE_TO_CPU(ahdr1.codec);
            ahdr1.frames            = BE_TO_CPU(ahdr1.frames);

            res                     = wr->write_header(&ahdr1);
            UTEST_ASSERT(res == STATUS_OK);
        }

        // Flush data
        res = wr->flush();
        UTEST_ASSERT(res == STATUS_OK);

        // Write contents of audio chunk
        size_t chunk_id     = wr->unique_id();
        res                 = wr->write(content, content.size());
        UTEST_ASSERT(res == STATUS_OK);

        // Flush data
        res = wr->flush();
        UTEST_ASSERT(res == STATUS_OK);

        // Close chunk
        res = wr->close();
        UTEST_ASSERT(res == STATUS_OK);
        delete wr;

        // Write profile chunk
        wr = fd.write_chunk(LSPC_CHUNK_PROFILE);
        UTEST_ASSERT(wr != NULL);

        if (v2)
        {
            lspc_chunk_audio_profile_v2_t prof2;
            bzero(&prof2, sizeof(prof2));

            prof2.common.version    = 2;
            prof2.common.size       = sizeof(lspc_chunk_audio_profile_v2_t);
            prof2.chunk_id          = lspc::chunk_id_t(chunk_id);
            prof2.chirp_order       = 1;
            prof2.alpha             = 1.0;
            prof2.beta              = 2.0;
            prof2.gamma             = 3.0;
            prof2.delta             = 4.0;
            prof2.initial_freq      = 10.0;
            prof2.final_freq        = 48000.0;

            prof2.chunk_id          = CPU_TO_BE(prof2.chunk_id);
            prof2.chirp_order       = CPU_TO_BE(prof2.chirp_order);
            prof2.alpha             = CPU_TO_BE(prof2.alpha);
            prof2.beta              = CPU_TO_BE(prof2.beta);
            prof2.gamma             = CPU_TO_BE(prof2.gamma);
            prof2.delta             = CPU_TO_BE(prof2.delta);
            prof2.initial_freq      = CPU_TO_BE(prof2.initial_freq);
            prof2.final_freq        = CPU_TO_BE(prof2.final_freq);

            for (size_t i=0; i < EXTRA_SIZE; ++i)
                prof2.extra[i]          = uint8_t(i + 1);

            res                     = wr->write_header(&prof2);
            UTEST_ASSERT(res == STATUS_OK);
        }
        else
        {
            lspc::chunk_audio_profile_t prof1;
            bzero(&prof1, sizeof(prof1));

            prof1.common.version    = 1;
            prof1.common.size       = sizeof(lspc::chunk_audio_profile_t);
            prof1.chunk_id          = lspc::chunk_id_t(chunk_id);
            prof1.chirp_order       = 1;
            prof1.alpha             = 1.0;
            prof1.beta              = 2.0;
            prof1.gamma             = 3.0;
            prof1.delta             = 4.0;
            prof1.initial_freq      = 10.0;
            prof1.final_freq        = 48000.0;

            prof1.chunk_id          = CPU_TO_BE(prof1.chunk_id);
            prof1.chirp_order       = CPU_TO_BE(prof1.chirp_order);
            prof1.alpha             = CPU_TO_BE(prof1.alpha);
            prof1.beta              = CPU_TO_BE(prof1.beta);
            prof1.gamma             = CPU_TO_BE(prof1.gamma);
            prof1.delta             = CPU_TO_BE(prof1.delta);
            prof1.initial_freq      = CPU_TO_BE(prof1.initial_freq);
            prof1.final_freq        = CPU_TO_BE(prof1.final_freq);

            res                     = wr->write_header(&prof1);
            UTEST_ASSERT(res == STATUS_OK);
        }

        // Flush data
        res = wr->flush();
        UTEST_ASSERT(res == STATUS_OK);

        // Close header
        res             = wr->close();
        UTEST_ASSERT(res == STATUS_OK);
        delete wr;

        // Close file
        res             = fd.close();
        UTEST_ASSERT(res == STATUS_OK);
    }

    void read_lspc_file(ByteBuffer &content, bool v2)
    {
        lspc::File fd;
        io::Path path;

        UTEST_ASSERT(path.fmt("%s/utest-%s.lspc", tempdir(), full_name()));

        printf("Reading file %s ...\n", path.as_native());
        status_t res    = fd.open(&path);

        UTEST_ASSERT(res == STATUS_OK);

        // Find profile chunk
        uint32_t chunk_id   = 0;
        lspc::ChunkReader *rd = fd.find_chunk(LSPC_CHUNK_PROFILE, &chunk_id);
        UTEST_ASSERT_MSG(rd != NULL, "Error while searching for profile chunk");

        if (v2)
        {
            // Read as 'upgraded' version
            lspc_chunk_audio_profile_v2_t prof2;
            ssize_t n               = rd->read_header(&prof2, sizeof(lspc_chunk_audio_profile_v2_t));
            UTEST_ASSERT( n >= 0 );
            res                     = status_t(rd->last_error());
            UTEST_ASSERT_MSG((res == STATUS_OK) || (res == STATUS_EOF), "Invalid last_error status: %d", int(res));

            prof2.chunk_id          = CPU_TO_BE(prof2.chunk_id);
            prof2.chirp_order       = CPU_TO_BE(prof2.chirp_order);
            prof2.alpha             = CPU_TO_BE(prof2.alpha);
            prof2.beta              = CPU_TO_BE(prof2.beta);
            prof2.gamma             = CPU_TO_BE(prof2.gamma);
            prof2.delta             = CPU_TO_BE(prof2.delta);
            prof2.initial_freq      = CPU_TO_BE(prof2.initial_freq);
            prof2.final_freq        = CPU_TO_BE(prof2.final_freq);

            UTEST_ASSERT(prof2.chirp_order == 1);
            UTEST_ASSERT(prof2.alpha == 1.0);
            UTEST_ASSERT(prof2.beta == 2.0);
            UTEST_ASSERT(prof2.gamma == 3.0);
            UTEST_ASSERT(prof2.delta == 4.0);
            UTEST_ASSERT(prof2.initial_freq == 10.0);
            UTEST_ASSERT(prof2.final_freq == 48000.0);

            if (prof2.common.version == 1)
            {
                UTEST_ASSERT(prof2.common.size == sizeof(lspc::chunk_audio_profile_t));
                for (size_t i=0; i < EXTRA_SIZE; ++i)
                    UTEST_ASSERT(prof2.extra[i] == 0);
            }
            else if (prof2.common.version == 2)
            {
                UTEST_ASSERT(prof2.common.size == sizeof(lspc_chunk_audio_profile_v2_t));
                for (size_t i=0; i < EXTRA_SIZE; ++i)
                    UTEST_ASSERT(prof2.extra[i] == uint8_t(i + 1));
            }
            else
            {
                UTEST_FAIL_MSG("Invalid profile header version: %d", int(prof2.common.version));
            }

            chunk_id                = prof2.chunk_id;
        }
        else
        {
            // Read as 'legacy' version
            lspc::chunk_audio_profile_t prof1;
            ssize_t n               = rd->read_header(&prof1, sizeof(lspc::chunk_audio_profile_t));
            UTEST_ASSERT( n >= 0 );
            res                     = status_t(rd->last_error());
            UTEST_ASSERT_MSG((res == STATUS_OK) || (res == STATUS_EOF), "Invalid last_error status: %d", int(res));

            prof1.chunk_id          = CPU_TO_BE(prof1.chunk_id);
            prof1.chirp_order       = CPU_TO_BE(prof1.chirp_order);
            prof1.alpha             = CPU_TO_BE(prof1.alpha);
            prof1.beta              = CPU_TO_BE(prof1.beta);
            prof1.gamma             = CPU_TO_BE(prof1.gamma);
            prof1.delta             = CPU_TO_BE(prof1.delta);
            prof1.initial_freq      = CPU_TO_BE(prof1.initial_freq);
            prof1.final_freq        = CPU_TO_BE(prof1.final_freq);

            UTEST_ASSERT(prof1.chirp_order == 1);
            UTEST_ASSERT(prof1.alpha == 1.0);
            UTEST_ASSERT(prof1.beta == 2.0);
            UTEST_ASSERT(prof1.gamma == 3.0);
            UTEST_ASSERT(prof1.delta == 4.0);
            UTEST_ASSERT(prof1.initial_freq == 10.0);
            UTEST_ASSERT(prof1.final_freq == 48000.0);

            UTEST_ASSERT(prof1.common.size == sizeof(lspc::chunk_audio_profile_t));
            UTEST_ASSERT_MSG((prof1.common.version == 1) || (prof1.common.version == 2),
                    "Invalid profile header version: %d", int(prof1.common.version));

            chunk_id                = prof1.chunk_id;
        }

        // Try to read data after profile header
        ssize_t count   = rd->read(content.data(), content.size());
        UTEST_ASSERT_MSG(count <= 0, "Extra data found after profile header");
        res = status_t(rd->last_error());
        UTEST_ASSERT_MSG((res == STATUS_OK) || (res == STATUS_EOF), "Invalid last_error status: %d", int(res));

        // Close chunk reader
        res             = rd->close();
        UTEST_ASSERT(res == STATUS_OK);
        delete rd;

        // Read audio chunk
        rd              = fd.read_chunk(chunk_id);
        UTEST_ASSERT_MSG(rd != NULL, "Error while reading audio chunk id=%d", int(chunk_id));
        UTEST_ASSERT(rd->unique_id() == chunk_id);
        UTEST_ASSERT(rd->magic() == LSPC_CHUNK_AUDIO);

        if (v2)
        {
            // Read as 'upgraded' version
            lspc_chunk_audio_header_v2_t ahdr2;
            bzero(&ahdr2, sizeof(ahdr2));

            ssize_t n               = rd->read_header(&ahdr2, sizeof(lspc_chunk_audio_header_v2_t));
            UTEST_ASSERT(n >= 0);
            res                     = status_t(rd->last_error());
            UTEST_ASSERT_MSG((res == STATUS_OK), "Invalid last_error status: %d", int(res));

            ahdr2.channels          = BE_TO_CPU(ahdr2.channels);
            ahdr2.sample_format     = BE_TO_CPU(ahdr2.sample_format);
            ahdr2.sample_rate       = BE_TO_CPU(ahdr2.sample_rate);
            ahdr2.codec             = BE_TO_CPU(ahdr2.codec);
            ahdr2.frames            = BE_TO_CPU(ahdr2.frames);

            UTEST_ASSERT(ahdr2.channels == 1);
            UTEST_ASSERT(ahdr2.sample_format == lspc::SAMPLE_FMT_F32LE);
            UTEST_ASSERT(ahdr2.sample_rate == 48000);
            UTEST_ASSERT(ahdr2.codec == lspc::CODEC_PCM);
            UTEST_ASSERT(ahdr2.frames == (content.size() / sizeof(float)));

            if (ahdr2.common.version == 1)
            {
                UTEST_ASSERT(ahdr2.common.size == sizeof(lspc::chunk_audio_header_t));
                for (size_t i=0; i < EXTRA_SIZE; ++i)
                    UTEST_ASSERT(ahdr2.extra[i] == 0);
            }
            else if (ahdr2.common.version == 2)
            {
                UTEST_ASSERT(ahdr2.common.size == sizeof(lspc_chunk_audio_header_v2_t));
                for (size_t i=0; i < EXTRA_SIZE; ++i)
                    UTEST_ASSERT(ahdr2.extra[i] == uint8_t(i + 1));
            }
            else
            {
                UTEST_FAIL_MSG("Invalid profile header version: %d", int(ahdr2.common.version));
            }
        }
        else
        {
            // Read as 'legacy' version
            lspc::chunk_audio_header_t ahdr1;
            bzero(&ahdr1, sizeof(ahdr1));

            ssize_t n               = rd->read_header(&ahdr1, sizeof(lspc::chunk_audio_header_t));
            UTEST_ASSERT(n >= 0);
            res                     = status_t(rd->last_error());
            UTEST_ASSERT_MSG((res == STATUS_OK), "Invalid last_error status: %d", int(res));

            ahdr1.channels          = BE_TO_CPU(ahdr1.channels);
            ahdr1.sample_format     = BE_TO_CPU(ahdr1.sample_format);
            ahdr1.sample_rate       = BE_TO_CPU(ahdr1.sample_rate);
            ahdr1.codec             = BE_TO_CPU(ahdr1.codec);
            ahdr1.frames            = BE_TO_CPU(ahdr1.frames);

            UTEST_ASSERT(ahdr1.channels == 1);
            UTEST_ASSERT(ahdr1.sample_format == lspc::SAMPLE_FMT_F32LE);
            UTEST_ASSERT(ahdr1.sample_rate == 48000);
            UTEST_ASSERT(ahdr1.codec == lspc::CODEC_PCM);
            UTEST_ASSERT(ahdr1.frames == (content.size() / sizeof(float)));

            UTEST_ASSERT(ahdr1.common.size == sizeof(lspc::chunk_audio_header_t));
            UTEST_ASSERT_MSG((ahdr1.common.version == 1) || (ahdr1.common.version == 2),
                    "Invalid profile header version: %d", int(ahdr1.common.version));
        }

        // Read buffer data
        count = rd->read(content.data(), content.size());
        UTEST_ASSERT_MSG((count == ssize_t(content.size())),
                "Invalid read size: %d, expected: %d", int(count), int(content.size()));

        // Try to read data after profile header
        count = rd->read(content.data(), content.size());
        UTEST_ASSERT_MSG(count <= 0, "Invalid read size: %d, expected: %d", int(count), int(0));
        res = status_t(rd->last_error());
        UTEST_ASSERT_MSG((res == STATUS_OK) || (res == STATUS_EOF), "Invalid last_error status: %d", int(res));

        // Close chunk reader
        res             = rd->close();
        UTEST_ASSERT(res == STATUS_OK);
        delete rd;

        // Close file
        res             = fd.close();
        UTEST_ASSERT(res == STATUS_OK);
    }


    UTEST_MAIN
    {
        ByteBuffer src(BUFFER_SIZE);
        for (size_t i=0; i<=0x03; ++i)
        {
            printf("Writing %s data, reading %s data\n",
                    ((i & 0x01) ? "v2" : "v1"),
                    ((i & 0x02) ? "v2" : "v1")
                   );

            ByteBuffer dst(BUFFER_SIZE);
            create_lspc_file(src, i & 0x01);
            read_lspc_file(dst, i & 0x02);

            UTEST_ASSERT_MSG(src.valid(), "Source buffer corrupted");
            UTEST_ASSERT_MSG(dst.valid(), "Destination buffer corrupted");

            if (!src.equals(dst))
            {
                src.dump("src");
                dst.dump("dst");
                UTEST_FAIL_MSG("Source and destination buffers differ");
            }
        }
    }

UTEST_END



