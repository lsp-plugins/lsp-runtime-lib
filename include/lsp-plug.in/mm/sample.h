/*
 * sample.h
 *
 *  Created on: 16 апр. 2020 г.
 *      Author: sadko
 */

#ifndef LSP_PLUG_IN_MM_SAMPLE_H_
#define LSP_PLUG_IN_MM_SAMPLE_H_

#include <lsp-plug.in/runtime/version.h>
#include <lsp-plug.in/mm/types.h>

namespace lsp
{
    namespace mm
    {
        /**
         * Convert endianess of samples to match CPU endianess
         * @param buf buffer to process
         * @param samples number of samples to process
         * @param format sample format
         * @return true if conversion is possible
         */
        bool sample_endian_to_cpu(void *buf, size_t samples, size_t format);

        /**
         * Convert sample format
         * @param dst destination buffer to store samples
         * @param src source buffer to convert samples (contents may be modified during processing)
         * @param samples number of samples to convert
         * @param to target sample format
         * @param from source sample format
         * @return true if conversion is possible
         */
        bool convert_samples(void *dst, void *src, size_t samples, size_t to, size_t from);
    }
}


#endif /* LSP_PLUG_IN_MM_SAMPLE_H_ */
