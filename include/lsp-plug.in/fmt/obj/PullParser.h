/*
 * PullParser.h
 *
 *  Created on: 21 апр. 2017 г.
 *      Author: sadko
 */

#ifndef LSP_PLUG_IN_FMT_OBJ_PULLPARSER_H_
#define LSP_PLUG_IN_FMT_OBJ_PULLPARSER_H_

#include <lsp-plug.in/runtime/version.h>
#include <lsp-plug.in/common/types.h>
#include <lsp-plug.in/common/status.h>

#include <lsp-plug.in/io/IInSequence.h>
#include <lsp-plug.in/io/IInStream.h>
#include <lsp-plug.in/io/Path.h>

#include <lsp-plug.in/fmt/obj/const.h>

namespace lsp
{
    namespace obj
    {
        class PullParser
        {
            private:
                PullParser & operator = (const PullParser &);

            public:
                static const size_t IO_BUF_SIZE         = 8192;

            protected:
                io::IInSequence    *pIn;
                size_t              nWFlags;
                event_t             sEvent;

            protected:
                status_t            read_event();
                status_t            copy_event(event_t *ev);

            public:
                explicit PullParser();
                ~PullParser();

            public:
                /**
                 * Open parser
                 * @param path UTF-8 path to the file
                 * @param charset character set
                 * @return status of operation
                 */
                status_t            open(const char *path, const char *charset = NULL);

                /**
                 * Open parser
                 * @param path string representation of path to the file
                 * @param charset character set
                 * @return status of operation
                 */
                status_t            open(const LSPString *path, const char *charset = NULL);

                /**
                 * Open parser
                 * @param path path to the file
                 * @param charset character set
                 * @return status of operation
                 */
                status_t            open(const io::Path *path, const char *charset = NULL);

                /**
                 * Wrap string with parser
                 * @param str string to wrap
                 * @param charset character set
                 * @return status of operation
                 */
                status_t            wrap(const char *str, const char *charset = NULL);

                /**
                 * Wrap string with parser
                 * @param str string to wrap
                 * @return status of operation
                 */
                status_t            wrap(const LSPString *str);

                /**
                 * Wrap input sequence with parser
                 * @param seq sequence to use for reads
                 * @return status of operation
                 */
                status_t            wrap(io::IInSequence *seq, size_t flags = WRAP_NONE);

                /**
                 * Wrap input stream with parser
                 * @param is input stream
                 * @param flags wrap flags
                 * @param charset character set
                 * @return status of operation
                 */
                status_t            wrap(io::IInStream *is, size_t flags = WRAP_NONE, const char *charset = NULL);

                /**
                 * Close parser
                 * @return status of operation
                 */
                status_t            close();

            public:
                /**
                 * Get next event
                 * @param ev pointer to structure to store the event
                 * @return status of operation
                 */
                status_t            next(event_t *ev = NULL);

                /**
                 * Get current event
                 * @param ev pointer to structure to store the event
                 * @return NULL if there is no current event
                 */
                const event_t      *current() const;

//            protected:
//                typedef struct file_buffer_t
//                {
//                    io::IInSequence    *in;
//                    LSPString           line;
//                    lsp_wchar_t        *data;
//                    size_t              off;
//                    size_t              len;
//                    bool                skip_wc;
//                } file_buffer_t;
//
//                struct ofp_point3d_t: public point3d_t
//                {
//                    ssize_t     oid;    // Object identifier
//                    ssize_t     idx;    // Point index
//                };
//
//                struct ofp_vector3d_t: public vector3d_t
//                {
//                    ssize_t     oid;    // Object identifier
//                    ssize_t     idx;    // Vector index
//                };
//
//                typedef struct parse_state_t
//                {
//                    IObjHandler             *pHandler;
//                    ssize_t                     nObjectID;
//                    ssize_t                     nPointID;
//                    ssize_t                     nFaceID;
//                    ssize_t                     nLineID;
//                    size_t                      nLines;
//
//                    cstorage<ofp_point3d_t>     sVx;
//                    cstorage<ofp_point3d_t>     sTexVx;
//                    cstorage<ofp_point3d_t>     sParVx;
//                    cstorage<ofp_vector3d_t>    sNorm;
//
//                    cstorage<ssize_t>           sVxIdx;
//                    cstorage<ssize_t>           sTexVxIdx;
//                    cstorage<ssize_t>           sNormIdx;
//                } parse_state_t;

//            protected:
//                static void eliminate_comments(LSPString *s);
//
//                static status_t read_line(file_buffer_t *fb);
//
//                static status_t parse_lines(file_buffer_t *fb, IObjHandler *handler);
//
//                static status_t parse_line(parse_state_t *st, const char *s);
//
//                static status_t parse_finish(parse_state_t *st);
//
//                static const char *skip_spaces(const char *s);
//
//                static inline bool is_space(char ch);
//
//                static inline bool end_of_line(const char *s);
//
//                static inline bool prefix_match(const char *s, const char *prefix);
//
//                static bool parse_float(float *dst, const char **s);
//
//                static bool parse_int(ssize_t *dst, const char **s);
//
//            public:
//                static status_t parse(const char *path, IObjHandler *handler);
//
//                static status_t parse(const LSPString *path, IObjHandler *handler);
//
//                static status_t parse(const io::Path *path, IObjHandler *handler);
        };
    }
} /* namespace lsp */

#endif /* LSP_PLUG_IN_FMT_OBJ_PULLPARSER_H_ */
