/*
 * ACMStream.h
 *
 *  Created on: 24 апр. 2020 г.
 *      Author: sadko
 */

#ifndef PRIVATE_MM_ACMSTREAM_H_
#define PRIVATE_MM_ACMSTREAM_H_

#include <lsp-plug.in/runtime/version.h>
#include <lsp-plug.in/common/types.h>
#include <lsp-plug.in/common/status.h>
#include <lsp-plug.in/lltl/darray.h>
#include <lsp-plug.in/lltl/parray.h>

#ifndef USE_LIBSNDFILE

#include <mmreg.h>
#include <msacm.h>

namespace lsp
{
    namespace mm
    {
        /**
         * This is Windows-specific only data conversion class
         * responsible for multimedia conversion
         */
        class ACMStream
        {
            private:
                ACMStream & operator = (const ACMStream &);

            public:
                static const size_t     IO_BUF_SIZE     = 0x2000;

            protected:
                // ACM Format descriptor
                typedef struct fmt_t
                {
                    size_t                      id;         // Format index
                    size_t                      fdw;        // Format flags
                    WAVEFORMATEX               *wfex;       // Format descriptor
                } fmt_t;

                // ACM Format tag
                typedef struct fmt_tag_t
                {
                    size_t                      id;         // Tag index
                    size_t                      fdw;        // Format flags
                    LSPString                   name;       // Tag name
                    lltl::darray<fmt_t>         vfmt;       // List of formats
                } fmt_tag_t;

                // ACM driver
                typedef struct drv_t
                {
                    HACMDRIVERID                drv_id;     // ACM Driver identifier
                    LSPString                   short_name;
                    LSPString                   full_name;
                    LSPString                   copyright;
                    LSPString                   license;
                    LSPString                   features;
                    lltl::parray<fmt_tag_t>     vtag;       // List of tags
                } drv_t;

                typedef struct drv_lookup_t
                {
                    ACMDRIVERDETAILSW       dd;         // Driver details
                    HACMDRIVER              hd;         // Driver handle
                    ACMFORMATTAGDETAILSW    ftd;        // Details for current format tag
                    ACMFORMATDETAILSW       fd;         // Format details
                    WAVEFORMATEX           *req;        // Requested format details
                    DWORD                   req_sz;     // Requested format maximum size

                    lltl::parray<drv_t>     vdrv;       // Complete list of drivers
                    drv_t                  *pdrv;       // Current driver
                    fmt_tag_t              *ptag;       // Current tag
                    fmt_t                  *pfmt;       // Current format
                } drv_lookup_t;

            protected:
                WAVEFORMATEX       *pFmtIn;
                WAVEFORMATEX       *pFmtOut;
                HACMDRIVER          hDriver;
                HACMSTREAM          hStream;
                ACMSTREAMHEADER    *pHeader;
                lltl::parray<drv_t> vDrv;

            protected:
                static WAVEFORMATEX *copy_fmt(const WAVEFORMATEX *src);

            protected:
                static WINBOOL CALLBACK acm_driver_enum_cb(HACMDRIVERID hadid, DWORD_PTR dwInstance, DWORD fdwSupport);

                static void acm_query_format_tags(drv_lookup_t *s);
                static void acm_query_formats(drv_lookup_t *s);
                static status_t acm_enum_drivers(lltl::parray<drv_t> *res);
                static void acm_destroy_drivers(lltl::parray<drv_t> *res);
                static ACMSTREAMHEADER *acm_create_header(size_t in, size_t out);
                static fmt_tag_t *acm_find_tag(drv_t *drv, size_t fmt_tag);

                status_t acm_configure_stream(WAVEFORMATEX *dst, WAVEFORMATEX *src);
                status_t acm_try_format(WAVEFORMATEX *test, WAVEFORMATEX *fmt);
                status_t acm_find_nonstandard_dec(WAVEFORMATEX *fmt);
                status_t acm_find_standard_dec(WAVEFORMATEX *fmt);

            public:
                explicit ACMStream();
                ~ACMStream();

            public:
                /**
                 * Get input stream format
                 * @return input stream format
                 */
                inline WAVEFORMATEX *in_format()        { return pFmtIn; }

                /**
                 * Get output stream format
                 * @return output stream format
                 */
                inline WAVEFORMATEX *out_format()       { return pFmtOut; }

                /**
                 * Reserve space for writing data
                 * @param buf pointer to return pointer to the buffer (may be NULL)
                 * @return number of bytes available for write or negative error code
                 */
                ssize_t push(void **buf);

                /**
                 * Perform conversion and fetch data into buffer
                 * @param buf pointer to buffer to store data
                 * @param size number of bytes available in the buffer
                 * @param force retrieve last (final) portion of data
                 * @return number of bytes stored into buffer or negative error code
                 */
                ssize_t pull(void *buf, size_t size, bool force);

                /**
                 * Get number of bytes available for pulling
                 * @return number of bytes available for pulling
                 */
                size_t avail();

                /**
                 * Initialize ACM stream for reading PCM stream described
                 * by the specific format
                 * @param in the input stream format
                 * @return status of operation
                 */
                status_t read_pcm(WAVEFORMATEX *in);

                /**
                 * Initialize ACM stream for writing PCM stream described
                 * by the specific format
                 * @param out the output stream format
                 * @return status of operation
                 */
                status_t write_pcm(WAVEFORMATEX *out);

                /**
                 * Close ACM stream
                 * @param res the result to return
                 * @return status of operation
                 */
                status_t close(status_t res = STATUS_OK);
        };

    } /* namespace mm */
} /* namespace lsp */

#endif /* USE_LIBSNDFILE */

#endif /* PRIVATE_MM_ACMSTREAM_H_ */
