/*
 * Copyright (C) 2022 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2022 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-runtime-lib
 * Created on: 24 окт. 2022 г.
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

#include <lsp-plug.in/fmt/lspc/util/audio.h>
#include <lsp-plug.in/fmt/lspc/AudioWriter.h>
#include <lsp-plug.in/fmt/lspc/IAudioFormatSelector.h>
#include <lsp-plug.in/mm/InAudioFileStream.h>
#include <lsp-plug.in/stdlib/stdlib.h>

namespace lsp
{
    namespace lspc
    {
        static IAudioFormatSelector default_selector;

        LSP_RUNTIME_LIB_PUBLIC
        status_t write_audio(chunk_id_t *chunk_id, File *file, const char *path, IAudioFormatSelector *selector, size_t buf_size)
        {
            mm::InAudioFileStream is;
            status_t res = is.open(path);
            if (res != STATUS_OK)
                return res;
            res = write_audio(chunk_id, file, &is, selector, buf_size);
            status_t res2 = is.close();
            return (res != STATUS_OK) ? res2 : res;
        }

        LSP_RUNTIME_LIB_PUBLIC
        status_t write_audio(chunk_id_t *chunk_id, File *file, const io::Path *path, IAudioFormatSelector *selector, size_t buf_size)
        {
            mm::InAudioFileStream is;
            status_t res = is.open(path);
            if (res != STATUS_OK)
                return res;
            res = write_audio(chunk_id, file, &is, selector, buf_size);
            status_t res2 = is.close();
            return (res != STATUS_OK) ? res2 : res;
        }

        LSP_RUNTIME_LIB_PUBLIC
        status_t write_audio(chunk_id_t *chunk_id, File *file, const LSPString *path, IAudioFormatSelector *selector, size_t buf_size)
        {
            mm::InAudioFileStream is;
            status_t res = is.open(path);
            if (res != STATUS_OK)
                return res;
            res = write_audio(chunk_id, file, &is, selector, buf_size);
            status_t res2 = is.close();
            return (res != STATUS_OK) ? res2 : res;
        }

        LSP_RUNTIME_LIB_PUBLIC
        status_t write_audio(chunk_id_t *chunk_id, File *file, mm::IInAudioStream *is, IAudioFormatSelector *selector, size_t buf_size)
        {
            status_t res;
            if (is == NULL)
                return STATUS_BAD_ARGUMENTS;

            // Select audio format
            audio_format_t ofmt;
            mm::audio_stream_t ifmt;
            if ((res = is->info(&ifmt)) != STATUS_OK)
                return res;
            if (selector == NULL)
                selector            = &default_selector;
            if ((res = selector->decide(&ofmt, &ifmt)) != STATUS_OK)
                return res;

            // Create audio writer
            audio_parameters_t params;
            AudioWriter wr;
            params.channels     = ifmt.channels;
            params.frames       = ifmt.frames;
            params.sample_rate  = ofmt.sample_rate;
            params.codec        = ofmt.codec;
            params.sample_format= ofmt.sample_format;
            if ((res = wr.open(file, &params, false)) != STATUS_OK)
                return res;
            lsp_finally { wr.close(); };
            chunk_id_t res_chunk_id = wr.unique_id();

            // Allocate buffer for I/O
            size_t min_buf_size = ifmt.channels * sizeof(float);
            buf_size            = lsp_max(min_buf_size, buf_size - (buf_size) % min_buf_size);
            float *data         = static_cast<float *>(malloc(buf_size));
            if (data == NULL)
                return STATUS_NO_MEM;
            lsp_finally { free(data); };
            ssize_t max_frames  = buf_size / (ifmt.channels * sizeof(float));

            // Perform data copy
            for (wssize_t frame = 0; frame < ifmt.frames; )
            {
                size_t to_do        = lsp_min(max_frames, ifmt.frames - frame);
                ssize_t nread       = is->read(data, to_do);
                if (nread < 0)
                    return -nread;
                if ((res = wr.write_frames(data, nread)) != STATUS_OK)
                    return res;
                frame              += nread;
            }

            // Close the audio chunk writer
            if ((res = wr.close()) != STATUS_OK)
                return res;

            // Return success result
            if (chunk_id != NULL)
                *chunk_id           = res_chunk_id;
            return STATUS_OK;
        }

        LSP_RUNTIME_LIB_PUBLIC
        status_t write_audio(
            chunk_id_t *chunk_id, File *file,
            const float *frames, const audio_parameters_t *params)
        {
            status_t res;
            if ((frames == NULL) || (params == NULL))
                return STATUS_BAD_ARGUMENTS;

            // Create audio writer
            AudioWriter wr;
            if ((res = wr.open(file, params, false)) != STATUS_OK)
                return res;
            lsp_finally { wr.close(); };
            chunk_id_t res_chunk_id = wr.unique_id();

            // Perform data write
            if ((res = wr.write_frames(frames, params->frames)) != STATUS_OK)
                return res;

            // Close the audio chunk writer
            if ((res = wr.close()) != STATUS_OK)
                return res;

            // Return success result
            if (chunk_id != NULL)
                *chunk_id           = res_chunk_id;
            return STATUS_OK;
        }
    } /* namespace lspc */
} /* namespace lsp */



