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
            // TODO
            return STATUS_OK;
        }
    } /* namespace obj */
} /* namespace lsp */
