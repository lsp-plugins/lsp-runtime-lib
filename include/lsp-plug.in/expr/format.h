/*
 * format.h
 *
 *  Created on: 25 февр. 2020 г.
 *      Author: sadko
 */

#ifndef LSP_PLUG_IN_EXPR_FORMAT_H_
#define LSP_PLUG_IN_EXPR_FORMAT_H_

#include <lsp-plug.in/runtime/version.h>
#include <lsp-plug.in/io/IOutSequence.h>
#include <lsp-plug.in/io/IInSequence.h>
#include <lsp-plug.in/expr/Parameters.h>

namespace lsp
{
    namespace expr
    {
        /**
         * Format the set of parameters provided by Resolver using input character
         * sequence as format specifier and output the result to output character
         * sequence
         *
         * @param out output character sequence
         * @param fmt format specifier
         * @param r parameter resolver
         * @return status of operation
         */
        status_t format(io::IOutSequence *out, io::IInSequence *fmt, const Parameters *r);

        /**
         * Format the set of parameters provided by Resolver using format string
         * and output the result to output character sequence
         *
         * @param out output character sequence
         * @param fmt format specifier (UTF-8 character sequence)
         * @param r parameter resolver
         * @return status of operation
         */
        status_t format(io::IOutSequence *out, const char *fmt, const Parameters *r);

        /**
         * Format the set of parameters provided by Resolver using format string
         * and output the result to output character sequence
         *
         * @param out output character sequence
         * @param fmt format specifier
         * @param r parameter resolver
         * @return status of operation
         */
        status_t format(io::IOutSequence *out, const LSPString *fmt, const Parameters *r);

        /**
         * Format the set of parameters provided by Resolver using input character
         * sequence as format specifier and output the result to string
         *
         * @param out output string
         * @param fmt format specifier
         * @param r parameter resolver
         * @return status of operation
         */
        status_t format(LSPString *out, io::IInSequence *fmt, const Parameters *r);

        /**
         * Format the set of parameters provided by Resolver using format string
         * and output the result to string
         *
         * @param out output string
         * @param fmt format specifier (UTF-8 character sequence)
         * @param r parameter resolver
         * @return status of operation
         */
        status_t format(LSPString *out, const char *fmt, const Parameters *r);

        /**
         * Format the set of parameters provided by Resolver using format string
         * and output the result to string
         *
         * @param out output string
         * @param fmt format specifier
         * @param r parameter resolver
         * @return status of operation
         */
        status_t format(LSPString *out, const LSPString *fmt, const Parameters *r);
    }
}


#endif /* LSP_PLUG_IN_EXPR_FORMAT_H_ */
