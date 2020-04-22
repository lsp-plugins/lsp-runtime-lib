/*
 * PushParser.cpp
 *
 *  Created on: 22 апр. 2020 г.
 *      Author: sadko
 */

#include <lsp-plug.in/fmt/obj/PushParser.h>

namespace lsp
{
    namespace obj
    {
        
        PushParser::PushParser()
        {
        }
        
        PushParser::~PushParser()
        {
        }
    
        status_t PushParser::parse_file(IObjHandler *handler, const char *path, const char *charset)
        {
            IObjHandler stub;
            status_t res = sParser.open(path, charset);
            if (res == STATUS_OK)
                res = parse_document((handler != NULL) ? handler : &stub);
            if (res == STATUS_OK)
                res = sParser.close();
            else
                sParser.close();
            return res;
        }

        status_t PushParser::parse_file(IObjHandler *handler, const LSPString *path, const char *charset)
        {
            IObjHandler stub;
            status_t res = sParser.open(path, charset);
            if (res == STATUS_OK)
                res = parse_document((handler != NULL) ? handler : &stub);
            if (res == STATUS_OK)
                res = sParser.close();
            else
                sParser.close();
            return res;
        }

        status_t PushParser::parse_file(IObjHandler *handler, const io::Path *path, const char *charset)
        {
            IObjHandler stub;
            status_t res = sParser.open(path, charset);
            if (res == STATUS_OK)
                res = parse_document((handler != NULL) ? handler : &stub);
            if (res == STATUS_OK)
                res = sParser.close();
            else
                sParser.close();
            return res;
        }

        status_t PushParser::parse_data(IObjHandler *handler, io::IInStream *is, size_t flags, const char *charset)
        {
            IObjHandler stub;
            status_t res = sParser.wrap(is, flags, charset);
            if (res == STATUS_OK)
                res = parse_document((handler != NULL) ? handler : &stub);
            if (res == STATUS_OK)
                res = sParser.close();
            else
                sParser.close();
            return res;
        }

        status_t PushParser::parse_data(IObjHandler *handler, const char *str, const char *charset)
        {
            IObjHandler stub;
            status_t res = sParser.wrap(str, charset);
            if (res == STATUS_OK)
                res = parse_document((handler != NULL) ? handler : &stub);
            if (res == STATUS_OK)
                res = sParser.close();
            else
                sParser.close();
            return res;
        }

        status_t PushParser::parse_data(IObjHandler *handler, const LSPString *str)
        {
            IObjHandler stub;
            status_t res = sParser.wrap(str);
            if (res == STATUS_OK)
                res = parse_document((handler != NULL) ? handler : &stub);
            if (res == STATUS_OK)
                res = sParser.close();
            else
                sParser.close();
            return res;
        }

        status_t PushParser::parse_data(IObjHandler *handler, io::IInSequence *seq, size_t flags)
        {
            IObjHandler stub;
            status_t res = sParser.wrap(seq, flags);
            if (res == STATUS_OK)
                res = parse_document((handler != NULL) ? handler : &stub);
            if (res == STATUS_OK)
                res = sParser.close();
            else
                sParser.close();
            return res;
        }

        status_t PushParser::parse_document(IObjHandler *handler)
        {
            status_t res = STATUS_OK;
            size_t nobj = 0;

            while ((res = sParser.next()) == STATUS_OK)
            {
                // Fetch event
                const event_t *ev = sParser.current();
                if (ev == NULL)
                    return STATUS_BAD_STATE;

                // Analyze event
                switch (ev->type)
                {
                    case EV_OBJECT:
                        if (nobj++)
                        {
                            if ((res = handler->end_object()) != STATUS_OK)
                                return res;
                        }
                        if ((res = handler->begin_object(&ev->name)) != STATUS_OK)
                            return res;
                        break;

                    case EV_VERTEX:
                        if ((res = handler->add_vertex(ev->vertex.x, ev->vertex.y, ev->vertex.z, ev->vertex.w)) != STATUS_OK)
                            return res;
                        break;

                    case EV_PVERTEX:
                        if ((res = handler->add_param_vertex(ev->vertex.x, ev->vertex.y, ev->vertex.z, ev->vertex.w)) != STATUS_OK)
                            return res;
                        break;

                    case EV_NORMAL:
                        if ((res = handler->add_normal(ev->normal.dx, ev->normal.dy, ev->normal.dz, ev->normal.dw)) != STATUS_OK)
                            return res;
                        break;

                    case EV_TEXCOORD:
                        if ((res = handler->add_texture_vertex(ev->texcoord.u, ev->texcoord.v, ev->texcoord.w)) != STATUS_OK)
                            return res;
                        break;

                    case EV_FACE:
                        if ((res = handler->add_face(ev->ivertex.array(), ev->inormal.array(), ev->itexcoord.array(), ev->ivertex.size())) != STATUS_OK)
                            return res;
                        break;

                    case EV_LINE:
                        if ((res = handler->add_line(ev->ivertex.array(), ev->itexcoord.array(), ev->ivertex.size())) != STATUS_OK)
                            return res;
                        break;

                    case EV_POINT:
                        if ((res = handler->add_points(ev->ivertex.array(), ev->ivertex.size())) != STATUS_OK)
                            return res;
                        break;

                    default:
                        return STATUS_UNSUPPORTED_FORMAT;
                }
            }

            if (res == STATUS_EOF)
                res = STATUS_OK;

            // Post-process data
            if (res == STATUS_OK)
            {
                if (nobj)
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
