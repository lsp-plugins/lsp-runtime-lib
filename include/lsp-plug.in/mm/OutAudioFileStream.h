/*
 * OutAudioFileStream.h
 *
 *  Created on: 20 апр. 2020 г.
 *      Author: sadko
 */

#ifndef LSP_PLUG_IN_MM_OUTAUDIOFILESTREAM_H_
#define LSP_PLUG_IN_MM_OUTAUDIOFILESTREAM_H_

#include <lsp-plug.in/runtime/version.h>
#include <lsp-plug.in/runtime/LSPString.h>
#include <lsp-plug.in/io/Path.h>
#include <lsp-plug.in/mm/IOutAudioStream.h>

#ifdef USE_LIBSNDFILE
    #include <sndfile.h>
#endif

namespace lsp
{
    namespace mm
    {
        class OutAudioFileStream: public IOutAudioStream
        {
            private:
                OutAudioFileStream & operator = (const OutAudioFileStream &);

            protected:
                size_t              nCodec;

            #ifdef USE_LIBSNDFILE
                SNDFILE            *hHandle;
                bool                bSeekable;
            #endif

            protected:
            #ifdef USE_LIBSNDFILE
                static status_t     decode_sf_error(SNDFILE *fd);
                static bool         select_sndfile_format(SF_INFO *info, audio_stream_t *fmt, size_t codec);
            #endif

                virtual ssize_t     direct_write(const void *src, size_t nframes, size_t fmt);

                virtual size_t      select_format(size_t rfmt);

                status_t            close_handle();

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
