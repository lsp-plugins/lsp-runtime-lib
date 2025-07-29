/*
 * Copyright (C) 2025 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2025 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-runtime-lib
 * Created on: 23 июл. 2025 г.
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

#include <lsp-plug.in/test-fw/utest.h>
#include <lsp-plug.in/test-fw/helpers.h>
#include <lsp-plug.in/fmt/obj/Compressor.h>
#include <lsp-plug.in/fmt/obj/Decompressor.h>
#include <lsp-plug.in/fmt/obj/PushParser.h>
#include <lsp-plug.in/lltl/parray.h>
#include <lsp-plug.in/runtime/LSPString.h>

namespace lsp
{
    namespace
    {
        enum call_type_t
        {
            EV_BEGIN_OBJECT,
            EV_END_OBJECT,
            EV_END_OF_DATA,
            EV_ADD_VERTEX,
            EV_ADD_PARAM_VERTEX,
            EV_ADD_NORMAL,
            EV_ADD_TEXTURE_VERTEX,
            EV_ADD_FACE,
            EV_ADD_POINTS,
            EV_ADD_LINE
        };

        typedef struct call_t
        {
            call_type_t type;
            union
            {
                struct { float x, y, z, w;      } vertex;
                struct { float dx, dy, dz, dw;  } normal;
                struct { float u, v, w;         } texcoord;
                char *name;
                struct {
                    size_t              count;
                    obj::index_t       *v;
                    obj::index_t       *t;
                    obj::index_t       *n;
                } indices;
            };
        } call_t;

        class ObjRecorder: public obj::IObjHandler
        {
            protected:
                lltl::parray<call_t> vCalls;

            protected:
                call_t *add_call(call_type_t type, size_t bytes)
                {
                    call_t *call        = reinterpret_cast<call_t *>(malloc(sizeof(call_t) + bytes));
                    if (call == NULL)
                        return NULL;
                    if (!vCalls.add(call))
                    {
                        free(call);
                        return NULL;
                    }

                    bzero(call, sizeof(call_t));
                    call->type          = type;

                    switch (type)
                    {
                        case EV_BEGIN_OBJECT:
                            call->name              = reinterpret_cast<char *>(&call[1]);
                            break;
                        case EV_ADD_FACE:
                            call->indices.v         = reinterpret_cast<obj::index_t *>(&call[1]);
                            call->indices.t         = &call->indices.v[bytes / (3 * sizeof(obj::index_t))];
                            call->indices.n         = &call->indices.t[bytes / (3 * sizeof(obj::index_t))];
                            break;

                        case EV_ADD_POINTS:
                            call->indices.v         = reinterpret_cast<obj::index_t *>(&call[1]);
                            break;

                        case EV_ADD_LINE:
                            call->indices.v         = reinterpret_cast<obj::index_t *>(&call[1]);
                            call->indices.t         = &call->indices.v[bytes / (2 * sizeof(obj::index_t))];
                            break;

                        default:
                            break;
                    }

                    return call;
                }

            public:
                ObjRecorder()
                {
                }

                virtual ~ObjRecorder() override
                {
                    for (size_t i=0, n = vCalls.size(); i<n; ++i)
                    {
                        call_t *call = vCalls.uget(i);
                        free(call);
                    }
                    vCalls.flush();
                }

            public:
                virtual status_t begin_object(const char *name) override
                {
                    const size_t bytes = strlen(name) + 1;
                    call_t *call    = add_call(EV_BEGIN_OBJECT, bytes);
                    if (call == NULL)
                        return STATUS_NO_MEM;
                    memcpy(call->name, name, bytes);
                    return STATUS_OK;
                }

                virtual status_t begin_object(const LSPString *name) override
                {
                    return begin_object(name->get_utf8());
                }

                virtual status_t end_object() override
                {
                    return (add_call(EV_END_OBJECT, 0) != NULL) ? STATUS_OK : STATUS_NO_MEM;
                }

                virtual status_t end_of_data() override
                {
                    return (add_call(EV_END_OF_DATA, 0) != NULL) ? STATUS_OK : STATUS_NO_MEM;
                }

                virtual ssize_t add_vertex(float x, float y, float z, float w) override
                {
                    call_t *call        = add_call(EV_ADD_VERTEX, 0);
                    if (call == NULL)
                        return STATUS_NO_MEM;

                    call->vertex.x      = x;
                    call->vertex.y      = y;
                    call->vertex.z      = z;
                    call->vertex.w      = w;

                    return STATUS_OK;
                }

                virtual ssize_t add_param_vertex(float x, float y, float z, float w) override
                {
                    call_t *call        = add_call(EV_ADD_PARAM_VERTEX, 0);
                    if (call == NULL)
                        return STATUS_NO_MEM;

                    call->vertex.x      = x;
                    call->vertex.y      = y;
                    call->vertex.z      = z;
                    call->vertex.w      = w;

                    return STATUS_OK;
                }

                virtual ssize_t add_normal(float nx, float ny, float nz, float nw) override
                {
                    call_t *call        = add_call(EV_ADD_NORMAL, 0);
                    if (call == NULL)
                        return STATUS_NO_MEM;

                    call->normal.dx     = nx;
                    call->normal.dy     = ny;
                    call->normal.dz     = nz;
                    call->normal.dw     = nw;

                    return STATUS_OK;
                }

                virtual ssize_t add_texture_vertex(float u, float v, float w) override
                {
                    call_t *call        = add_call(EV_ADD_TEXTURE_VERTEX, 0);
                    if (call == NULL)
                        return STATUS_NO_MEM;

                    call->texcoord.u    = u;
                    call->texcoord.v    = v;
                    call->texcoord.w    = w;

                    return STATUS_OK;
                }

                virtual ssize_t add_face(const obj::index_t *vv, const obj::index_t *vn, const obj::index_t *vt, size_t n) override
                {
                    call_t *call        = add_call(EV_ADD_FACE, n * sizeof(obj::index_t) * 3);
                    if (call == NULL)
                        return STATUS_NO_MEM;

                    call->indices.count     = n;
                    memcpy(call->indices.v, vv, n * sizeof(obj::index_t));
                    memcpy(call->indices.t, vt, n * sizeof(obj::index_t));
                    memcpy(call->indices.n, vn, n * sizeof(obj::index_t));

                    return STATUS_OK;
                }

                virtual ssize_t add_points(const obj::index_t *vv, size_t n) override
                {
                    call_t *call        = add_call(EV_ADD_POINTS, n * sizeof(obj::index_t));
                    if (call == NULL)
                        return STATUS_NO_MEM;

                    call->indices.count     = n;
                    memcpy(call->indices.v, vv, n * sizeof(obj::index_t));

                    return STATUS_OK;
                }

                virtual ssize_t add_line(const obj::index_t *vv, const obj::index_t *vt, size_t n) override
                {
                    call_t *call        = add_call(EV_ADD_LINE, n * sizeof(obj::index_t) * 2);
                    if (call == NULL)
                        return STATUS_NO_MEM;

                    call->indices.count     = n;
                    memcpy(call->indices.v, vv, n * sizeof(obj::index_t));
                    memcpy(call->indices.t, vt, n * sizeof(obj::index_t));

                    return STATUS_OK;
                }

            public:
                inline const call_t *get(size_t i) const
                {
                    return vCalls.get(i);
                }


        };
    }
} /* namespace lsp */

UTEST_BEGIN("runtime.fmt.obj", compressor)

    status_t read_obj_file(ObjRecorder *data, const io::Path *path)
    {
        obj::PushParser parser;
        return parser.parse_file(data, path);
    }

    status_t read_compressed_obj_file(ObjRecorder *data, const io::Path *path)
    {
        obj::Decompressor parser;
        return parser.parse_file(data, path);
    }

    ssize_t compare(const ObjRecorder *a, const ObjRecorder *b)
    {
        for (size_t i=0; ; ++i)
        {
            const call_t *ca = a->get(i);
            const call_t *cb = b->get(i);

            if (ca == NULL)
            {
                if (cb == NULL)
                    return -1;
                printf("Missing record #%d for second file\n", int(i));
                return i;
            }
            else if (cb == NULL)
            {
                printf("Missing record #%d for first file\n", int(i));
                return i;
            }

            if (ca->type != cb->type)
            {
                printf("Record #%d type %d does not match record type %d\n",
                    int(i), int(ca->type), int(cb->type));
                return i;
            }

            switch (ca->type)
            {
                case EV_BEGIN_OBJECT:
                    if (strcmp(ca->name, cb->name) != 0)
                    {
                        printf("Record #%d object name '%s' does not match object name '%s'\n",
                            int(i), ca->name, cb->name);
                        return i;
                    }
                    break;

                case EV_ADD_VERTEX:
                    if ((ca->vertex.x != cb->vertex.x) ||
                        (ca->vertex.y != cb->vertex.y) ||
                        (ca->vertex.z != cb->vertex.z) ||
                        (ca->vertex.w != cb->vertex.w))
                    {
                        printf("Record #%d vertex {%f, %f, %f, %f} does not match vertex {%f, %f, %f, %f}\n",
                            int(i),
                            ca->vertex.x, ca->vertex.y, ca->vertex.z, ca->vertex.w,
                            cb->vertex.x, cb->vertex.y, cb->vertex.z, cb->vertex.w);
                        return i;
                    }
                    break;

                case EV_ADD_PARAM_VERTEX:
                    if ((ca->vertex.x != cb->vertex.x) ||
                        (ca->vertex.y != cb->vertex.y) ||
                        (ca->vertex.z != cb->vertex.z) ||
                        (ca->vertex.w != cb->vertex.w))
                    {
                        printf("Record #%d parameter vertex {%f, %f, %f, %f} does not match parameter vertex {%f, %f, %f, %f}\n",
                            int(i),
                            ca->vertex.x, ca->vertex.y, ca->vertex.z, ca->vertex.w,
                            cb->vertex.x, cb->vertex.y, cb->vertex.z, cb->vertex.w);
                        return i;
                    }
                    break;

                case EV_ADD_NORMAL:
                    if ((ca->normal.dx != cb->normal.dx) ||
                        (ca->normal.dy != cb->normal.dy) ||
                        (ca->normal.dz != cb->normal.dz) ||
                        (ca->normal.dw != cb->normal.dw))
                    {
                        printf("Record #%d normal {%f, %f, %f, %f} does not match normal {%f, %f, %f, %f}\n",
                            int(i),
                            ca->normal.dx, ca->normal.dy, ca->normal.dz, ca->normal.dw,
                            cb->normal.dx, cb->normal.dy, cb->normal.dz, cb->normal.dw);
                        return i;
                    }
                    break;

                case EV_ADD_TEXTURE_VERTEX:
                    if ((ca->texcoord.u != cb->texcoord.u) ||
                        (ca->texcoord.v != cb->texcoord.v) ||
                        (ca->texcoord.w != cb->texcoord.w))
                    {
                        printf("Record #%d normal {%f, %f, %f} does not match normal {%f, %f, %f}\n",
                            int(i),
                            ca->texcoord.u, ca->texcoord.v, ca->texcoord.w,
                            cb->texcoord.u, cb->texcoord.v, cb->texcoord.w);
                        return i;
                    }
                    break;

                case EV_ADD_FACE:
                    if (ca->indices.count != cb->indices.count)
                    {
                        printf("Record #%d number of face indices %d does not match %d\n",
                            int(i), int(ca->indices.count), int(cb->indices.count));
                        return i;
                    }
                    for (size_t j=0; i<ca->indices.count; ++i)
                    {
                        if (ca->indices.v[j] != cb->indices.v[j])
                        {
                            printf("Record #%d face vertex index #%d value %d differs to %d\n",
                                int(i), int(j), int(ca->indices.v[j]), int(cb->indices.v[j]));
                            return i;
                        }
                    }
                    for (size_t j=0; i<ca->indices.count; ++i)
                    {
                        if (ca->indices.t[j] != cb->indices.t[j])
                        {
                            printf("Record #%d face texture index #%d value %d differs to %d\n",
                                int(i), int(j), int(ca->indices.t[j]), int(cb->indices.t[j]));
                            return i;
                        }
                    }
                    for (size_t j=0; i<ca->indices.count; ++i)
                    {
                        if (ca->indices.n[j] != cb->indices.n[j])
                        {
                            printf("Record #%d face normal index #%d value %d differs to %d\n",
                                int(i), int(j), int(ca->indices.n[j]), int(cb->indices.n[j]));
                            return i;
                        }
                    }
                    break;

                case EV_ADD_POINTS:
                    if (ca->indices.count != cb->indices.count)
                    {
                        printf("Record #%d number of point indices %d does not match %d\n",
                            int(i), int(ca->indices.count), int(cb->indices.count));
                        return i;
                    }
                    for (size_t j=0; i<ca->indices.count; ++i)
                    {
                        if (ca->indices.v[j] != cb->indices.v[j])
                        {
                            printf("Record #%d point vertex index #%d value %d differs to %d\n",
                                int(i), int(j), int(ca->indices.v[j]), int(cb->indices.v[j]));
                            return i;
                        }
                    }
                    break;

                case EV_ADD_LINE:
                    if (ca->indices.count != cb->indices.count)
                    {
                        printf("Record #%d number of line indices %d does not match %d\n",
                            int(i), int(ca->indices.count), int(cb->indices.count));
                        return i;
                    }

                    for (size_t j=0; i<ca->indices.count; ++i)
                    {
                        if (ca->indices.v[j] != cb->indices.v[j])
                        {
                            printf("Record #%d line vertex index #%d value %d differs to %d\n",
                                int(i), int(j), int(ca->indices.v[j]), int(cb->indices.v[j]));
                            return i;
                        }
                    }
                    for (size_t j=0; i<ca->indices.count; ++i)
                    {
                        if (ca->indices.t[j] != cb->indices.t[j])
                        {
                            printf("Record #%d line texture index #%d value %d differs to %d\n",
                                int(i), int(j), int(ca->indices.t[j]), int(cb->indices.t[j]));
                            return i;
                        }
                    }

                    break;

                case EV_END_OF_DATA:
                case EV_END_OBJECT:
                default:
                    break;
            }
        }

        return -1;
    }

    void test_compress_obj_file(const char *fname, const char *postfix)
    {
        io::Path src, dst;
        UTEST_ASSERT(src.fmt("%s/%s", resources(), fname) > 0);
        UTEST_ASSERT(dst.fmt("%s/utest-%s-%s.cobj", tempdir(), full_name(), postfix) > 0);

        printf("Compressing %s -> %s\n", src.as_native(), dst.as_native());

        obj::Compressor c;
        obj::PushParser p;

        UTEST_ASSERT(c.set_buffer_size(7) == STATUS_OK);
        UTEST_ASSERT(c.open(&dst, io::File::FM_WRITE_NEW) == STATUS_OK);
        UTEST_ASSERT(p.parse_file(&c, &src) == STATUS_OK);
        UTEST_ASSERT(c.close() == STATUS_OK);

        const wssize_t src_size = src.size();
        UTEST_ASSERT(src_size >= 0);
        const wssize_t dst_size = dst.size();
        UTEST_ASSERT(dst_size >= 0);

        printf("Stats: uncompressed size = %d, compressed size = %d, ratio: %.3f\n",
            int(src_size), int(dst_size),
            double(src_size) / double(dst_size));

        printf("Validating result...\n");
        ObjRecorder osrc, odst;

        status_t res = read_obj_file(&osrc, &src);
        UTEST_ASSERT_MSG(res == STATUS_OK, "read_obj_file failed with code=%d", int(res));
        res = read_compressed_obj_file(&odst, &dst);
        UTEST_ASSERT_MSG(res == STATUS_OK, "read_compressed_obj_file failed with code=%d", int(res));
        ssize_t diff = compare(&osrc, &odst);
        UTEST_ASSERT_MSG(diff < 0, "Source and destination records #%d differ", int(diff));
    }

    UTEST_MAIN
    {
        printf("Testing compression of complicated OBJ file...\n");
        test_compress_obj_file("fmt/obj/parking.obj", "parking");
        printf("Testing compression of complicated OBJ file...\n");
        test_compress_obj_file("fmt/obj/coliseum.obj", "coliseum");
        printf("Testing compression of complicated OBJ file...\n");
        test_compress_obj_file("fmt/obj/forest.obj", "forest");
        printf("Testing compression of complicated OBJ file...\n");
        test_compress_obj_file("fmt/obj/cooling-tower.obj", "cooling-tower");
    }

UTEST_END


