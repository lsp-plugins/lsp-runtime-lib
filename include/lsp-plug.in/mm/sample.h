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
        void convert_samples(void *dst, const void *src, size_t samples, sformat_t to, sformat_t from);
    }
}


#endif /* LSP_PLUG_IN_MM_SAMPLE_H_ */
