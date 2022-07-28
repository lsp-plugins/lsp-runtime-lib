/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-runtime-lib
 * Created on: 20 апр. 2020 г.
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

#ifndef LSP_PLUG_IN_MM_OUTAUDIOFILESTREAM_H_
#define LSP_PLUG_IN_MM_OUTAUDIOFILESTREAM_H_

#include <lsp-plug.in/runtime/version.h>
#include <lsp-plug.in/runtime/LSPString.h>
#include <lsp-plug.in/io/Path.h>
#include <lsp-plug.in/mm/IOutAudioStream.h>

#ifdef USE_LIBSNDFILE
    #include <sndfile.h>
#endif /* USE_LIBSNDFILE */

namespace lsp
{
    namespace mm
    {
    #ifndef USE_LIBSNDFILE
        struct WAVEFILE;
    #endif /* USE_LIBSNDFILE */

        class OutAudioFileStream: public IOutAudioStream
        {
            private:
                OutAudioFileStream & operator = (const OutAudioFileStream &);

            protected:

            #ifdef USE_LIBSNDFILE
                typedef SNDFILE                 handle_t;
            #else
                typedef struct WAVEFILE         handle_t;
            #endif

            protected:
                handle_t           *hHandle;
                size_t              nCodec;
                bool                bSeekable;

            protected:
            #ifdef USE_LIBSNDFILE
                static status_t     decode_sf_error(SNDFILE *fd);
                static bool         select_sndfile_format(SF_INFO *info, audio_stream_t *fmt, size_t codec);
            #else
                virtual ssize_t     conv_write(const void *src, size_t nframes, size_t fmt);
                ssize_t             write_acm_convert(const void *src, size_t nframes);
                status_t            flush_handle(handle_t *hHandle, bool eof);
            #endif

                virtual ssize_t     direct_write(const void *src, size_t nframes, size_t fmt);

                virtual size_t      select_format(size_t rfmt);

                status_t            flush_internal(bool eof);
                status_t            do_close();
                static status_t     close_handle(handle_t *h);

            public:
                explicit OutAudioFileStream();
                virtual ~OutAudioFileStream();

            public:
                /**
                 * Get codec used for output file
                 * @return codec used for output file
                 */
                inline size_t       codec() const       { return nCodec; }

                /**
                 * Open audio stream
                 * @param path path to the audio file
                 * @return status of operation
                 */
                virtual status_t    open(const char *path, const audio_stream_t *fmt, size_t codec);

                /**
                 * Open audio stream
                 * @param path path to the audio file
                 * @return status of operation
                 */
                virtual status_t    open(const LSPString *path, const audio_stream_t *fmt, size_t codec);

                /**
                 * Open audio stream
                 * @param path path to the audio file
                 * @return status of operation
                 */
                virtual status_t    open(const io::Path *path, const audio_stream_t *fmt, size_t codec);

                virtual status_t    close();

                virtual status_t    flush();

                virtual wssize_t    seek(wsize_t nframes);
        };
    
    } /* namespace mm */
} /* namespace lsp */

#endif /* LSP_PLUG_IN_MM_OUTAUDIOFILESTREAM_H_ */
