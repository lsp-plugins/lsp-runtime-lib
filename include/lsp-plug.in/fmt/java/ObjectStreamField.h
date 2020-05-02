/*
 * ObjectStreamField.h
 *
 *  Created on: 30 авг. 2019 г.
 *      Author: sadko
 */

#ifndef LSP_PLUG_IN_FMT_JAVA_OBJECTSTREAMFIELD_H_
#define LSP_PLUG_IN_FMT_JAVA_OBJECTSTREAMFIELD_H_

#include <lsp-plug.in/runtime/version.h>
#include <lsp-plug.in/runtime/LSPString.h>
#include <lsp-plug.in/fmt/java/const.h>
#include <lsp-plug.in/fmt/java/Object.h>
#include <lsp-plug.in/fmt/java/String.h>

namespace lsp
{
    namespace java
    {
        class ObjectStreamField: public Object
        {
            public:
                static const char *CLASS_NAME;

            private:
                friend class ObjectStream;
                ObjectStreamField & operator = (const ObjectStreamField &);

            protected:
                ftype_t         enType;
                LSPString       sName;
                String         *pSignature;
                size_t          nOffset;
                char           *sRawName;

            public:
                explicit ObjectStreamField();
                virtual ~ObjectStreamField();

            public:
                inline ftype_t              type() const { return enType; }
                inline const LSPString     *name() const { return &sName; }
                inline const char          *raw_name() const { return sRawName; }
                inline const LSPString     *signature() const { return (pSignature != NULL) ? pSignature->string() : NULL; }
                inline const String        *java_signature() const { return pSignature; }
                inline size_t               size_of() const { return java::size_of(enType); };
                inline size_t               aligned_offset(size_t offset) const { return java::aligned_offset(offset, enType); };
                inline bool                 is_reference() const { return java::is_reference(enType); }
                inline bool                 is_primitive() const { return java::is_primitive(enType); }
                inline size_t               offset() const { return nOffset; }
        };
    
    } /* namespace java */
} /* namespace lsp */

#endif /* LSP_PLUG_IN_FMT_JAVA_OBJECTSTREAMFIELD_H_ */
