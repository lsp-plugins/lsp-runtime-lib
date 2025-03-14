/*
 * Copyright (C) 2025 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2025 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-runtime-lib
 * Created on: 28 авг. 2019 г.
 *
 * lsp-runtime-lib is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * lsp-runtime-lib is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with lsp-runtime-lib. If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef LSP_PLUG_IN_FMT_JAVA_OBJECTSTREAM_H_
#define LSP_PLUG_IN_FMT_JAVA_OBJECTSTREAM_H_

#include <lsp-plug.in/runtime/version.h>
#include <lsp-plug.in/runtime/LSPString.h>
#include <lsp-plug.in/common/types.h>
#include <lsp-plug.in/common/status.h>
#include <lsp-plug.in/io/Path.h>
#include <lsp-plug.in/io/IInStream.h>

#include <lsp-plug.in/fmt/java/const.h>
#include <lsp-plug.in/fmt/java/Object.h>
#include <lsp-plug.in/fmt/java/String.h>
#include <lsp-plug.in/fmt/java/RawArray.h>
#include <lsp-plug.in/fmt/java/Enum.h>
#include <lsp-plug.in/fmt/java/Handles.h>
#include <lsp-plug.in/fmt/java/ObjectStreamField.h>
#include <lsp-plug.in/fmt/java/ObjectStreamClass.h>

namespace lsp
{
    namespace java
    {
        class ObjectStream
        {
            protected:
                typedef struct block_t
                {
                    uint8_t            *data;
                    size_t              size;
                    size_t              offset;
                    size_t              unread;
                    bool                enabled;
                } block_t;

            protected:
                io::IInStream      *pIS;
                size_t              nFlags;
                status_t            nToken;
                stream_token_t      enToken;
                size_t              nDepth;
                ssize_t             nVersion;
                Handles            *pHandles;
                block_t             sBlock;
                String             *vTypeStrings[JFT_TOTAL];

            private:
                status_t            do_close();
                status_t            set_block_mode(bool enabled, bool *old = NULL);
                status_t            intern_type_string(String **dst, ftype_t type, char ptype);

            protected:
                status_t    initial_read(io::IInStream *is);
                status_t    get_token();
                status_t    lookup_token();
                inline void clear_token();

                status_t    fill_block();
                status_t    read_fully(void *dst, size_t count);

                status_t    skip_block_data();
                status_t    skip_custom_data();
                status_t    read_custom_data(void **dst, size_t *size);
                status_t    parse_array(RawArray **dst);
                status_t    parse_enum(Enum **dst);
                status_t    parse_ordinary_object(Object **dst);
                status_t    parse_reset();
                status_t    parse_null(Object **dst);
                status_t    parse_class_field(ObjectStreamField **dst);
                status_t    parse_class_descriptor(ObjectStreamClass **dst);
                status_t    parse_proxy_class_descriptor(ObjectStreamClass **dst);
                status_t    parse_utf(LSPString *dst, size_t len);
                status_t    parse_reference(Object **dst, const char *type = NULL);
                status_t    parse_string(String **dst);
                status_t    parse_object(Object **dst);
                status_t    parse_serial_data(Object *dst, ObjectStreamClass *desc);
                status_t    parse_external_data(Object *dst, ObjectStreamClass *desc);

                Object             *build_object(ObjectStreamClass *desc);

                inline status_t     start_object(bool &mode);
                inline status_t     end_object(bool &mode, status_t res);

            public:
                explicit ObjectStream(Handles *handles);
                virtual ~ObjectStream();

            public:
                /**
                 * Open stream reader
                 * @param file location of the file in UTF-8 encoding
                 * @return status of operation
                 */
                status_t    open(const char *file);

                /**
                 * Open stream reader
                 * @param file location of the file
                 * @return status of operation
                 */
                status_t    open(const LSPString *file);

                /**
                 * Open stream reader
                 * @param file location of the file
                 * @return status of operation
                 */
                status_t    open(const io::Path *file);

                /**
                 * Wrap memory buffer with stream reader
                 * @param buf data contents
                 * @param count size of data
                 * @return status of operation
                 */
                status_t    wrap(const void *buf, size_t count);

                /**
                 * Wrap memory buffer with stream reader
                 * @param buf data contents
                 * @param count size of data
                 * @param drop drop operation on close()
                 * @return status of operation
                 */
                status_t    wrap(void *buf, size_t count, lsp_memdrop_t drop);

                /**
                 * Wrap input stream
                 * @param is input stream
                 * @param flags actions on the input stream after close()
                 * @return status of operation
                 */
                status_t    wrap(io::IInStream *is, size_t flags);

                /**
                 * Close data reader
                 * @return status of operation
                 */
                status_t    close();

                /**
                 * Get current object stream version
                 * @return current object stream version
                 */
                inline ssize_t version() const { return nVersion; }

                status_t    current_token();

                status_t    read_byte(uint8_t *dst);
                status_t    read_byte(int8_t *dst);
                status_t    read_short(uint16_t *dst);
                status_t    read_short(int16_t *dst);
                status_t    read_int(uint32_t *dst);
                status_t    read_int(int32_t *dst);
                status_t    read_long(uint64_t *dst);
                status_t    read_long(int64_t *dst);
                status_t    read_float(float_t *dst);
                status_t    read_double(double_t *dst);
                status_t    read_char(lsp_utf16_t *dst);
                status_t    read_bool(bool_t *dst);

                status_t    read_bytes(uint8_t *dst, size_t count);
                status_t    read_bytes(int8_t *dst, size_t count);
                status_t    read_shorts(uint16_t *dst, size_t count);
                status_t    read_shorts(int16_t *dst, size_t count);
                status_t    read_ints(uint32_t *dst, size_t count);
                status_t    read_ints(int32_t *dst, size_t count);
                status_t    read_longs(uint64_t *dst, size_t count);
                status_t    read_longs(int64_t *dst, size_t count);
                status_t    read_floats(float_t *dst, size_t count);
                status_t    read_doubles(double_t *dst, size_t count);
                status_t    read_chars(lsp_utf16_t *dst, size_t count);
                status_t    read_bools(bool_t *dst, size_t count);

                status_t    read_utf(LSPString *dst);

                status_t    read_object(Object **dst);
                status_t    read_array(RawArray **dst);
                status_t    read_string(String **dst);
                status_t    read_string(LSPString *dst);
                status_t    read_enum(Enum **dst);
                status_t    read_class_descriptor(ObjectStreamClass **dst);
        };
    
    } /* namespace java */
} /* namespace lsp */

#endif /* LSP_PLUG_IN_FMT_JAVA_OBJECTSTREAM_H_ */
