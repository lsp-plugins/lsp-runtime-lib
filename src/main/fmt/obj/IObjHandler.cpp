/*
 * IObjHandler.cpp
 *
 *  Created on: 21 апр. 2017 г.
 *      Author: sadko
 */

#include <lsp-plug.in/fmt/obj/IObjHandler.h>

namespace lsp
{
    namespace obj
    {
        IObjHandler::~IObjHandler()
        {
        }

        status_t IObjHandler::begin_object(const char *name)
        {
            return STATUS_OK;
        }

        status_t IObjHandler::begin_object(const LSPString *name)
        {
            return begin_object(name->get_utf8());
        }

        status_t IObjHandler::end_object()
        {
            return STATUS_OK;
        }

        status_t IObjHandler::end_of_data()
        {
            return STATUS_OK;
        }

        ssize_t IObjHandler::add_vertex(float x, float y, float z, float w)
        {
            return 0;
        }

        ssize_t IObjHandler::add_param_vertex(float x, float y, float z, float w)
        {
            return 0;
        }

        ssize_t IObjHandler::add_normal(float dx, float dy, float dz, float dn)
        {
            return 0;
        }

        ssize_t IObjHandler::add_texture_vertex(float u, float v, float w)
        {
            return 0;
        }

        status_t IObjHandler::add_face(const index_t *vv, const index_t *vn, const index_t *vt, size_t n)
        {
            return STATUS_OK;
        }

        status_t IObjHandler::add_points(const index_t *vv, size_t n)
        {
            return STATUS_OK;
        }

        status_t IObjHandler::add_line(const index_t *vv, const index_t *vt, size_t n)
        {
            return STATUS_OK;
        }
    }
} /* namespace lsp */
