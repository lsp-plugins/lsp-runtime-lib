/*
 * url.h
 *
 *  Created on: 21 дек. 2019 г.
 *      Author: sadko
 */

#ifndef LSP_PLUG_IN_FMT_URL_H_
#define LSP_PLUG_IN_FMT_URL_H_

#include <lsp-plug.in/runtime/version.h>
#include <lsp-plug.in/runtime/LSPString.h>
#include <lsp-plug.in/common/status.h>

namespace lsp
{
    namespace url
    {
        /**
         * Decode URL-encoded string
         * @param dst destination string to store decoded string
         * @param src source string to parse
         * @return status of operation
         */
        status_t    decode(LSPString *dst, const LSPString *src);

        /**
         * Decode URL-encoded string
         * @param dst destination string to store decoded string
         * @param src source string to parse
         * @param first first character of the source string to start parsing
         * @return status of operation
         */
        status_t    decode(LSPString *dst, const LSPString *src, size_t first);

        /**
         * Decode URL-encoded string
         * @param dst destination string to store decoded string
         * @param src source string to parse
         * @param first first character of the source string to start parsing
         * @param last last character of the source string to start parsing
         * @return status of operation
         */
        status_t    decode(LSPString *dst, const LSPString *src, size_t first, size_t last);
    }
}

#endif /* LSP_PLUG_IN_FMT_URL_H_ */
