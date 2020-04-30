/*
 * PushParser.cpp
 *
 *  Created on: 30 апр. 2020 г.
 *      Author: sadko
 */

#include <lsp-plug.in/fmt/config/PushParser.h>

namespace lsp
{
    namespace config
    {
        
        PushParser::PushParser()
        {
        }
        
        PushParser::~PushParser()
        {
        }

        status_t PushParser::parse_file(IConfigHandler *handler, const char *path, const char *charset)
        {
            IConfigHandler stub;
            status_t res = sParser.open(path, charset);
            if (res == STATUS_OK)
                res = parse_document((handler != NULL) ? handler : &stub);
            if (res == STATUS_OK)
                res = sParser.close();
            else
                sParser.close();
            return res;
        }

        status_t PushParser::parse_file(IConfigHandler *handler, const LSPString *path, const char *charset)
        {
            IConfigHandler stub;
            status_t res = sParser.open(path, charset);
            if (res == STATUS_OK)
                res = parse_document((handler != NULL) ? handler : &stub);
            if (res == STATUS_OK)
                res = sParser.close();
            else
                sParser.close();
            return res;
        }

        status_t PushParser::parse_file(IConfigHandler *handler, const io::Path *path, const char *charset)
        {
            IConfigHandler stub;
            status_t res = sParser.open(path, charset);
            if (res == STATUS_OK)
                res = parse_document((handler != NULL) ? handler : &stub);
            if (res == STATUS_OK)
                res = sParser.close();
            else
                sParser.close();
            return res;
        }

        status_t PushParser::parse_data(IConfigHandler *handler, io::IInStream *is, size_t flags, const char *charset)
        {
            IConfigHandler stub;
            status_t res = sParser.wrap(is, flags, charset);
            if (res == STATUS_OK)
                res = parse_document((handler != NULL) ? handler : &stub);
            if (res == STATUS_OK)
                res = sParser.close();
            else
                sParser.close();
            return res;
        }

        status_t PushParser::parse_data(IConfigHandler *handler, const char *str, const char *charset)
        {
            IConfigHandler stub;
            status_t res = sParser.wrap(str, charset);
            if (res == STATUS_OK)
                res = parse_document((handler != NULL) ? handler : &stub);
            if (res == STATUS_OK)
                res = sParser.close();
            else
                sParser.close();
            return res;
        }

        status_t PushParser::parse_data(IConfigHandler *handler, const LSPString *str)
        {
            IConfigHandler stub;
            status_t res = sParser.wrap(str);
            if (res == STATUS_OK)
                res = parse_document((handler != NULL) ? handler : &stub);
            if (res == STATUS_OK)
                res = sParser.close();
            else
                sParser.close();
            return res;
        }

        status_t PushParser::parse_data(IConfigHandler *handler, io::IInSequence *seq, size_t flags)
        {
            IConfigHandler stub;
            status_t res = sParser.wrap(seq, flags);
            if (res == STATUS_OK)
                res = parse_document((handler != NULL) ? handler : &stub);
            if (res == STATUS_OK)
                res = sParser.close();
            else
                sParser.close();
            return res;
        }

        status_t PushParser::parse_document(IConfigHandler *handler)
        {
            // Indicate start of parsing
            status_t res = handler->start();
            if (res != STATUS_OK)
                return res;

            // Main loop
            while ((res = sParser.next()) == STATUS_OK)
            {
                if ((res = handler->handle(sParser.current())) != STATUS_OK)
                    break;
            }

            if (res == STATUS_EOF)
                res = STATUS_OK;

            // Always indicate end of parsing
            return handler->finish(res);
        }
    
    } /* namespace config */
} /* namespace lsp */
