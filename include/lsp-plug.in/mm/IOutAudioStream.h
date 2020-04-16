/*
 * OutStream.h
 *
 *  Created on: 16 апр. 2020 г.
 *      Author: sadko
 */

#ifndef LSP_PLUG_IN_MM_IOUTAUDIOSTREAM_H_
#define LSP_PLUG_IN_MM_IOUTAUDIOSTREAM_H_

#include <lsp-plug.in/runtime/version.h>
#include <lsp-plug.in/common/status.h>
#include <lsp-plug.in/mm/types.h>

namespace lsp
{
    namespace mm
    {
        class IOutAudioStream
        {
            private:
                IOutAudioStream & operator = (const IOutAudioStream &);

            protected:
                status_t        nErrorCode;

            protected:
                inline status_t set_error(status_t error) { return nErrorCode = error; }

            public:
                explicit IOutAudioStream();
                virtual ~IOutAudioStream();
        };
    
    } /* namespace mm */
} /* namespace lsp */

#endif /* LSP_PLUG_IN_MM_IOUTAUDIOSTREAM_H_ */
