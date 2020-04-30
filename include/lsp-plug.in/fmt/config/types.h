/*
 * types.h
 *
 *  Created on: 29 апр. 2020 г.
 *      Author: sadko
 */

#ifndef LSP_PLUG_IN_FMT_CONFIG_TYPES_H_
#define LSP_PLUG_IN_FMT_CONFIG_TYPES_H_

#include <lsp-plug.in/runtime/version.h>
#include <lsp-plug.in/runtime/LSPString.h>

namespace lsp
{
    namespace config
    {
        enum serial_flags_t
        {
            SF_NONE         = 0,        //!< SF_NONE no flags

            SF_TYPE_NONE    = 0,
            SF_TYPE_I32     = 0x01,     //!< SF_TYPE_I32 Parameter is of int32 type
            SF_TYPE_U32     = 0x02,     //!< SF_TYPE_U32 Parameter is of uint32 type
            SF_TYPE_I64     = 0x03,     //!< SF_TYPE_I64 Parameter is of int64 type
            SF_TYPE_U64     = 0x04,     //!< SF_TYPE_U64 Parameter is of uint64 type
            SF_TYPE_F32     = 0x05,     //!< SF_TYPE_F32 Parameter is of float type
            SF_TYPE_F64     = 0x06,     //!< SF_TYPE_F64 Parameter is of double type
            SF_TYPE_STR     = 0x07,     //!< SF_TYPE_STR Parameter is of string type
            SF_TYPE_BLOB    = 0x08,     //!< SF_TYPE_BLOB Parameter is of blob type

            SF_TYPE_MASK    = 0x0f,     //!< SF_TYPE_MASK Data type mask

            SF_PREC_NORMAL  = 0x00,     //!< SF_PREC_NORMAL Normal precision
            SF_PREC_SHORT   = 0x10,     //!< SF_PREC_SHORT Short precision
            SF_PREC_LONG    = 0x20,     //!< SF_PREC_SHORT Long precision
            SF_PREC_SCI     = 0x30,     //!< SF_PREC_SCI Scientific presentation
            SF_PREC_MASK    = 0x30,     //!< SF_PREC_MASK Precision mask for floating-point values

            SF_QUOTED       = 1 << 8,   //!< SF_QUOTED Parameter is quoted
            SF_COMMENT      = 1 << 9,   //!< SF_COMMENT Parameter has comment
            SF_TYPE_SET     = 1 << 10,  //!< SF_TYPE_SET Explicitly specify type
            SF_DECIBELS     = 1 << 11,  //!< SF_DECIBELS Serialize value as decibels
        };

        /**
         * Blob data
         */
        typedef struct blob_t
        {
            size_t      length;         // Length of the BLOB
            char       *ctype;          // Content-type UTF-8 encoded
            char       *data;           // BLOB data present as UTF-8 string
        } blob_t;

        typedef union
        {
            int32_t         i32;
            uint32_t        u32;
            int64_t         i64;
            uint64_t        u64;
            float           f32;
            double          f64;
            char           *str;    // UTF-8 string value (for string)
            blob_t          blob;   // BLOB data
        } value_t;

        /**
         * Parameters stored in the configuration file
         */
        typedef struct param_t
        {
            private:
                param_t &operator = (const param_t &);

            public:
                LSPString   name;           // Name of parameter
                LSPString   comment;        // Comment
                size_t      flags;          // Serialization flags
                value_t     v;              // Value

            public:
                explicit param_t();
                ~param_t();

            public:
                bool            copy(const param_t *src);
                inline bool     copy(const param_t &src)    { return copy(&src); };

                void            swap(param_t *dst);
                inline void     swap(param_t &dst)          { swap(&dst); };

                void            clear();
        } param_t;
    }
}

#endif /* LSP_PLUG_IN_FMT_CONFIG_TYPES_H_ */
