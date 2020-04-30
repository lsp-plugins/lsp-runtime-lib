/*
 * IConfigHandler.h
 *
 *  Created on: 30 апр. 2020 г.
 *      Author: sadko
 */

#ifndef LSP_PLUG_IN_FMT_CONFIG_ICONFIGHANDLER_H_
#define LSP_PLUG_IN_FMT_CONFIG_ICONFIGHANDLER_H_

#include <lsp-plug.in/runtime/version.h>
#include <lsp-plug.in/common/status.h>
#include <lsp-plug.in/fmt/config/types.h>

namespace lsp
{
    namespace config
    {
        /**
         * Configuration handler
         */
        class IConfigHandler
        {
            private:
                IConfigHandler & operator = (const IConfigHandler &);

            public:
                explicit IConfigHandler();
                virtual ~IConfigHandler();

            public:
                /**
                 * Indicates begin of parsing config
                 * @return status of operation
                 */
                virtual status_t start();

                /**
                 * Indicates end of parsing config
                 * @param res final result
                 * @return status of operation
                 */
                virtual status_t finish(status_t res);

                /**
                 * Handle parameter. By default it dispatches calls to other handle_* methods
                 * @param param parameter to handle
                 * @return status of operation
                 */
                virtual status_t handle(const param_t *param);

                virtual status_t handle_i32(const LSPString *name, int32_t value, size_t flags);

                virtual status_t handle_u32(const LSPString *name, uint32_t value, size_t flags);

                virtual status_t handle_f32(const LSPString *name, float value, size_t flags);

                virtual status_t handle_i64(const LSPString *name, int64_t value, size_t flags);

                virtual status_t handle_u64(const LSPString *name, uint64_t value, size_t flags);

                virtual status_t handle_f64(const LSPString *name, double value, size_t flags);

                virtual status_t handle_string(const LSPString *name, const LSPString *value, size_t flags);

                virtual status_t handle_blob(const LSPString *name, const blob_t *value, size_t flags);
        };
    
    } /* namespace config */
} /* namespace lsp */

#endif /* LSP_PLUG_IN_FMT_CONFIG_ICONFIGHANDLER_H_ */
