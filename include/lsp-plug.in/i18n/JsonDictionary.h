/*
 * JsonDictionary.h
 *
 *  Created on: 26 февр. 2020 г.
 *      Author: sadko
 */

#ifndef LSP_PLUG_IN_I18N_JSONDICTIONARY_H_
#define LSP_PLUG_IN_I18N_JSONDICTIONARY_H_

#include <lsp-plug.in/runtime/version.h>
#include <lsp-plug.in/fmt/json/Parser.h>
#include <lsp-plug.in/i18n/IDictionary.h>
#include <lsp-plug.in/lltl/parray.h>

namespace lsp
{
    namespace i18n
    {
        class JsonDictionary: public IDictionary
        {
            private:
                JsonDictionary & operator = (const JsonDictionary &);

            protected:
                typedef struct node_t
                {
                    LSPString               sKey;
                    LSPString               sValue;
                    JsonDictionary         *pChild;
                } node_t;

            protected:
                lltl::parray<node_t>    vNodes;

            protected:
                status_t            add_node(const node_t *node);
                status_t            parse_json(json::Parser *p);
                node_t             *find_node(const LSPString *key);

            public:
                explicit JsonDictionary();
                virtual ~JsonDictionary();

            public:
                using IDictionary::init;
                using IDictionary::get_value;
                using IDictionary::get_child;
                using IDictionary::lookup;

                virtual status_t    init(const LSPString *path);

                virtual status_t    lookup(const LSPString *key, LSPString *value);

                virtual status_t    lookup(const LSPString *key, IDictionary **value);

                virtual status_t    get_value(size_t index, LSPString *key, LSPString *value);

                virtual status_t    get_child(size_t index, LSPString *key, IDictionary **dict);

                virtual size_t      size();
        };
    }

} /* namespace lsp */

#endif /* LSP_PLUG_IN_I18N_JSONDICTIONARY_H_ */
