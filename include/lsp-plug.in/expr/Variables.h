/*
 * Variables.h
 *
 *  Created on: 19 сент. 2019 г.
 *      Author: sadko
 */

#ifndef LSP_PLUG_IN_EXPR_VARIABLES_H_
#define LSP_PLUG_IN_EXPR_VARIABLES_H_

#include <lsp-plug.in/runtime/version.h>
#include <lsp-plug.in/runtime/LSPString.h>
#include <lsp-plug.in/expr/Resolver.h>
#include <lsp-plug.in/lltl/parray.h>

namespace lsp
{
    namespace expr
    {
        class Variables: public Resolver
        {
            private:
                Variables & operator = (const Variables &);

            protected:
                typedef struct variable_t
                {
                    LSPString                   name;
                    value_t                     value;
                } variable_t;

            protected:
                Resolver                   *pResolver;
                lltl::parray<variable_t>    vVars;

            protected:
                status_t            add(const LSPString *name, const value_t *value);

            public:
                explicit Variables();
                explicit Variables(Resolver *r);
                virtual ~Variables();

            public:
                virtual status_t    resolve(value_t *value, const char *name, size_t num_indexes = 0, const ssize_t *indexes = NULL);
                virtual status_t    resolve(value_t *value, const LSPString *name, size_t num_indexes = 0, const ssize_t *indexes = NULL);

            public:
                status_t            set_int(const char *name, ssize_t value);
                status_t            set_float(const char *name, double value);
                status_t            set_bool(const char *name, bool value);
                status_t            set_string(const char *name, const char *value, const char *charset = NULL);
                status_t            set_string(const char *name, const LSPString *value);
                status_t            set_null(const char *name);

                status_t            set_int(const LSPString *name, ssize_t value);
                status_t            set_float(const LSPString *name, double value);
                status_t            set_bool(const LSPString *name, bool value);
                status_t            set_string(const LSPString *name, const char *value, const char *charset = NULL);
                status_t            set_string(const LSPString *name, const LSPString *value);
                status_t            set_null(const LSPString *name);

                status_t            set(const char *name, const value_t *value);
                status_t            set(const LSPString *name, const value_t *value);
                status_t            unset(const char *name, value_t *value = NULL);
                status_t            unset(const LSPString *name, value_t *value = NULL);

                void                clear();

                /**
                 * Get variable resolver
                 * @return variable resolver
                 */
                inline Resolver    *resolver() { return pResolver; }

                /**
                 * Sett variable resolver
                 * @param resolver variable resolver
                 */
                inline void         set_resolver(Resolver *resolver) { pResolver = resolver; }
        };
    
    } /* namespace calc */
} /* namespace lsp */

#endif /* LSP_PLUG_IN_EXPR_VARIABLES_H_ */
