/*
 * Copyright (C) 2025 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2025 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-runtime-lib
 * Created on: 26 июл. 2025 г.
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

#include <lsp-plug.in/common/alloc.h>
#include <lsp-plug.in/fmt/obj/const.h>
#include <lsp-plug.in/fmt/obj/Decompressor.h>
#include <lsp-plug.in/io/InMemoryStream.h>

namespace lsp
{
    namespace obj
    {
        static const uint8_t event_codes[] =
        {
            CEV_VERTEX3,    // 0x00
            CEV_NORMAL3,    // 0x01
            CEV_FACE_N,     // 0x02
            CEV_FACE,       // 0x03

            CEV_TEXCOORD1,  // 0x08
            CEV_TEXCOORD2,  // 0x09
            CEV_TEXCOORD3,  // 0x0a
            CEV_OBJECT,     // 0x0b

            CEV_FACE_T,     // 0x18
            CEV_FACE_TN,    // 0x19
            CEV_PVERTEX3,   // 0x1a
            CEV_LINE,       // 0x1b

            CEV_LINE_T,     // 0x38
            CEV_POINT,      // 0x39
            CEV_VERTEX2,    // 0x3a
            CEV_NORMAL2,    // 0x3b

            CEV_VERTEX4,    // 0x78
            CEV_PVERTEX2,   // 0x79
            CEV_PVERTEX4,   // 0x7a
            CEV_NORMAL4,    // 0x7b

            CEV_EOF,        // 0xf8
            CEV_EOF,        // 0xf9
            CEV_EOF,        // 0xfa
            CEV_EOF,        // 0xfb
        };

        static inline float bin_to_float(int32_t v)
        {
            union {
                float f32;
                int32_t i32;
            } cvt;

            cvt.i32 = v;
            return cvt.f32;
        }

        Decompressor::Decompressor()
        {
            vFloatBuf   = NULL;

            nFloatHead  = 0;
            nFloatSize  = 0;
            nFloatCap   = 0;
            nFloatBits  = 0;

            nLastEvent  = compressed_event_type_t(-1);
        }

        Decompressor::~Decompressor()
        {
            clear_state();
        }

        void Decompressor::clear_state()
        {
            if (vFloatBuf != NULL)
            {
                free(vFloatBuf);
                vFloatBuf       = NULL;
            }
        }

        status_t Decompressor::parse_file(IObjHandler *handler, const char *path)
        {
            clear_state();
            status_t res = sStream.open(path);
            if (res == STATUS_OK)
                res = parse_data(handler);
            res = update_status(res, sStream.close());
            clear_state();
            return res;
        }

        status_t Decompressor::parse_file(IObjHandler *handler, const LSPString *path)
        {
            clear_state();
            status_t res = sStream.open(path);
            if (res == STATUS_OK)
                res = parse_data(handler);
            res = update_status(res, sStream.close());
            clear_state();
            return res;
        }

        status_t Decompressor::parse_file(IObjHandler *handler, const io::Path *path)
        {
            clear_state();
            status_t res = sStream.open(path);
            if (res == STATUS_OK)
                res = parse_data(handler);
            res = update_status(res, sStream.close());
            clear_state();
            return res;
        }

        status_t Decompressor::parse_data(IObjHandler *handler, io::IInStream *is, size_t flags)
        {
            clear_state();
            status_t res = sStream.wrap(is, flags);
            if (res == STATUS_OK)
                res = parse_data(handler);
            res = update_status(res, sStream.close());
            clear_state();
            return res;
        }

        status_t Decompressor::parse_data(IObjHandler *handler, const void *data, size_t size)
        {
            clear_state();
            io::InMemoryStream is;
            is.wrap(data, size);
            status_t res = sStream.wrap(&is, WRAP_CLOSE);
            if (res == STATUS_OK)
                res = parse_data(handler);
            res = update_status(res, sStream.close());
            clear_state();
            return res;
        }

        status_t Decompressor::parse_header()
        {
            compressed_header_t hdr;

            // Read header
            status_t res = sStream.read_fully(&hdr, sizeof(compressed_header_t));
            if (res != STATUS_OK)
                return res;

            // Validate header
            if (hdr.signature != COMPRESSED_SIGNATURE)
                return STATUS_BAD_FORMAT;
            if (hdr.version != 0)
                return STATUS_UNSUPPORTED_FORMAT;

            if ((hdr.float_bits > MAX_FLOAT_BUF_BITS) || (hdr.float_bits < MIN_FLOAT_BUF_BITS))
                return STATUS_CORRUPTED;
            if ((hdr.pad[0] != 'L') || (hdr.pad[1] != 'S'))
                return STATUS_BAD_FORMAT;

            // Check that there is nothing to change
            const size_t float_cap  = 1 << hdr.float_bits;
            float *ptr              = static_cast<float *>(malloc(float_cap * sizeof(float)));
            if (ptr == NULL)
                return STATUS_NO_MEM;

            vFloatBuf               = ptr;
            nFloatHead              = 0;
            nFloatSize              = 0;
            nFloatCap               = float_cap;
            nFloatBits              = uint32_t(hdr.float_bits);

            return STATUS_OK;
        }

        status_t Decompressor::read_event(compressed_event_type_t *event)
        {
            bool b;
            size_t group = 0;
            uint32_t cmd = 0;

            // Check if command has been repeated
            status_t res = sStream.readb(&b);
            if (res != STATUS_OK)
                return res;
            if (b)
            {
                *event  = nLastEvent;
                return STATUS_OK;
            }

            // Determine group of commands
            while ((res = sStream.readb(&b)) == STATUS_OK)
            {
                sStream.readb(&b);
                if (!b)
                    break;
                ++group;
            }

            // Read subcommand identifier
            if (res == STATUS_OK)
                res = sStream.readv(&cmd, 2);
            if (res == STATUS_OK)
            {
                // Convert subcommand + group to event code
                cmd     = (group << 2) | cmd;
                if (cmd >= sizeof(event_codes))
                    return STATUS_CORRUPTED;

                *event  = compressed_event_type_t(event_codes[cmd]);
            }

            return res;
        }

        status_t Decompressor::read_float(float *dst)
        {
            // Read index
            uint32_t index = 0;
            status_t res = sStream.readv(&index, nFloatBits);
            if (res != STATUS_OK)
                return res;

            // Analyze index
            const uint32_t base = nFloatHead + nFloatCap - 1;

            if (index < nFloatSize)
            {
                *dst    = vFloatBuf[(base - index) % nFloatCap];
                if (index > 0)
                {
                    // Advance position of item one step forward if it is not the first one
                    const uint32_t idx  = (base - index) % nFloatCap;
                    lsp::swap(vFloatBuf[idx], vFloatBuf[(idx + 1) % nFloatCap]);
                }

                return STATUS_OK;
            }

            float value;
            if (index == nFloatSize)
            {
                // Read index of base float
                if ((res = sStream.readv(&index, nFloatBits)) != STATUS_OK)
                    return res;
                // Read delta
                size_t delta;
                if ((res = read_varint(&delta)) != STATUS_OK)
                    return res;
                const int32_t dvalue    = (int32_t(delta) >> 1) ^ -(int32_t(delta) & 1); // Zig-zag reverse conversion

                // Compute floating-point value
                const int32_t image     = vIntBuf[(base - index) % nFloatCap] + dvalue;
                value                   = bin_to_float(image);
            }
            else
            {
                // Read just the floating-point value
                int32_t image;
                if ((res = sStream.readv(&image)) != STATUS_OK)
                    return res;
                value                   = bin_to_float(LE_TO_CPU(image));
            }

            // Push item to buffer and return result
            vFloatBuf[nFloatHead]   = value;
            nFloatHead              = (nFloatHead + 1) % nFloatCap;
            nFloatSize              = lsp_min(nFloatSize + 1, nFloatCap - 2);

            *dst                    = value;

            return STATUS_OK;
        }

        status_t Decompressor::read_varint(size_t *dst)
        {
            size_t value        = 0;
            uint8_t b;

            do
            {
                status_t res        = sStream.readv(&b, 7);
                if (res != STATUS_OK)
                    return res;

                value               = (value << 6) | (b & 0x3f);
            } while (b & 0x40);

            *dst                = value;

            return STATUS_OK;
        }

        status_t Decompressor::read_varint_icount(size_t *dst)
        {
            size_t bits         = 3;
            size_t max          = 1 << (bits + 1);
            size_t value        = 0;
            size_t b            = 0;

            while (true)
            {
                status_t res        = sStream.readv(fixed_int(&b), bits + 1);
                if (res != STATUS_OK)
                    return res;

                // Decode value
                value               = (value << bits) | (b & (max - 1));
                if (!(value & max))
                    break;

                // Update bit count
                bits               += 2;
                max               <<= 2;
            } while (value > 0);

            *dst                = value;

            return STATUS_OK;
        }

        status_t Decompressor::read_indices(lltl::darray<index_t> *dst, size_t count, bool read)
        {
            // TODO
            return STATUS_OK;
        }

        status_t Decompressor::read_utf8(LSPString *dst)
        {
            // TODO
            return STATUS_OK;
        }

        status_t Decompressor::parse_vertex(IObjHandler *handler, size_t coords)
        {
            float x = 0.0f, y = 0.0f, z = 0.0f, w = 1.0f;

            status_t res    = read_float(&x);
            if (res == STATUS_OK)
                res         = read_float(&y);
            if ((res == STATUS_OK) && (coords > 2))
                res         = read_float(&z);
            if ((res == STATUS_OK) && (coords > 3))
                res         = read_float(&w);
            if (res == STATUS_OK)
                res         = handler->add_vertex(x, y, z, w);

            return res;
        }

        status_t Decompressor::parse_pvertex(IObjHandler *handler, size_t coords)
        {
            float x = 0.0f, y = 0.0f, z = 0.0f, w = 1.0f;

            status_t res    = read_float(&x);
            if (res == STATUS_OK)
                res             = read_float(&y);
            if ((res == STATUS_OK) && (coords > 2))
                res             = read_float(&z);
            if ((res == STATUS_OK) && (coords > 3))
                res             = read_float(&w);
            if (res == STATUS_OK)
                res             = handler->add_param_vertex(x, y, z, w);

            return res;
        }

        status_t Decompressor::parse_normal(IObjHandler *handler, size_t coords)
        {
            float nx = 0.0f, ny = 0.0f, nz = 0.0f, nw = 0.0f;

            status_t res    = read_float(&nx);
            if (res == STATUS_OK)
                res             = read_float(&ny);
            if ((res == STATUS_OK) && (coords > 2))
                res             = read_float(&nz);
            if ((res == STATUS_OK) && (coords > 3))
                res             = read_float(&nw);
            if (res == STATUS_OK)
                res             = handler->add_normal(nx, ny, nz, nw);

            return res;
        }

        status_t Decompressor::parse_texcoord(IObjHandler *handler, size_t coords)
        {
            float u = 0.0f, v = 0.0f, w = 0.0f;

            status_t res    = read_float(&u);
            if ((res == STATUS_OK) && (coords > 1))
                res             = read_float(&v);
            if ((res == STATUS_OK) && (coords > 2))
                res             = read_float(&w);
            if (res == STATUS_OK)
                res             = handler->add_texture_vertex(u, v, w);
            return res;
        }

        status_t Decompressor::parse_face(IObjHandler *handler, bool texcoords, bool normals)
        {
            size_t count    = 0;
            status_t res    = read_varint(&count);
            if (res != STATUS_OK)
                return res;

            lltl::darray<index_t> data;
            if (!data.reserve(count * 3))
                return STATUS_NO_MEM;

            if ((res = read_indices(&data, count, true)) != STATUS_OK)
                return res;
            if ((res = read_indices(&data, count, texcoords)) != STATUS_OK)
                return res;
            if ((res = read_indices(&data, count, normals)) != STATUS_OK)
                return res;

            const index_t *vv = data.array();
            return handler->add_face(vv, &vv[count], &vv[count << 1], count);
        }

        status_t Decompressor::parse_line(IObjHandler *handler, bool texcoords)
        {
            size_t count    = 0;
            status_t res    = read_varint(&count);
            if (res != STATUS_OK)
                return res;

            lltl::darray<index_t> data;
            if (!data.reserve(count * 2))
                return STATUS_NO_MEM;

            if ((res = read_indices(&data, count, true)) != STATUS_OK)
                return res;
            if ((res = read_indices(&data, count, texcoords)) != STATUS_OK)
                return res;

            const index_t *vv = data.array();
            return handler->add_line(vv, &vv[count], count);
        }

        status_t Decompressor::parse_points(IObjHandler *handler)
        {
            size_t count    = 0;
            status_t res    = read_varint(&count);
            if (res != STATUS_OK)
                return res;

            lltl::darray<index_t> data;
            if (!data.reserve(count))
                return STATUS_NO_MEM;

            if ((res = read_indices(&data, count, true)) != STATUS_OK)
                return res;

            return handler->add_points(data.array(), count);
        }

        status_t Decompressor::parse_object(IObjHandler *handler)
        {
            LSPString name;
            status_t res    = read_utf8(&name);
            if (res != STATUS_OK)
                return res;

            return handler->begin_object(&name);
        }

        status_t Decompressor::parse_data(IObjHandler *handler)
        {
            status_t res = parse_header();
            if (res != STATUS_OK)
                return res;

            size_t nobj = 0;
            compressed_event_type_t event;
            while ((res = read_event(&event)) == STATUS_OK)
            {
                if (event == CEV_EOF)
                    break;

                switch (event)
                {
                    case CEV_VERTEX2:
                        res     = parse_vertex(handler, 2);
                        break;
                    case CEV_VERTEX3:
                        res     = parse_vertex(handler, 3);
                        break;
                    case CEV_VERTEX4:
                        res     = parse_vertex(handler, 4);
                        break;

                    case CEV_PVERTEX2:
                        res     = parse_pvertex(handler, 2);
                        break;
                    case CEV_PVERTEX3:
                        res     = parse_pvertex(handler, 3);
                        break;
                    case CEV_PVERTEX4:
                        res     = parse_pvertex(handler, 4);
                        break;

                    case CEV_TEXCOORD1:
                        res     = parse_texcoord(handler, 1);
                        break;
                    case CEV_TEXCOORD2:
                        res     = parse_texcoord(handler, 2);
                        break;
                    case CEV_TEXCOORD3:
                        res     = parse_texcoord(handler, 3);
                        break;

                    case CEV_NORMAL2:
                        res     = parse_normal(handler, 2);
                        break;
                    case CEV_NORMAL3:
                        res     = parse_normal(handler, 3);
                        break;
                    case CEV_NORMAL4:
                        res     = parse_normal(handler, 4);
                        break;

                    case CEV_FACE:
                        res     = parse_face(handler, false, false);
                        break;
                    case CEV_FACE_T:
                        res     = parse_face(handler, true, false);
                        break;
                    case CEV_FACE_N:
                        res     = parse_face(handler, false, true);
                        break;
                    case CEV_FACE_TN:
                        res     = parse_face(handler, true, true);
                        break;

                    case CEV_LINE:
                        res     = parse_line(handler, false);
                        break;
                    case CEV_LINE_T:
                        res     = parse_line(handler, true);
                        break;

                    case CEV_POINT:
                        res     = parse_points(handler);
                        break;

                    case CEV_OBJECT:
                        if (nobj++)
                        {
                            if ((res = handler->end_object()) != STATUS_OK)
                                return res;
                        }
                        res     = parse_object(handler);
                        break;

                    default:
                        return STATUS_CORRUPTED;
                }
            }

            // Post-process data
            if (res == STATUS_OK)
            {
                if (nobj > 0)
                {
                    if ((res = handler->end_object()) != STATUS_OK)
                        return res;
                }

                res = handler->end_of_data();
            }

            return res;
        }

    } /* namespace obj */
} /* namespace lsp */


