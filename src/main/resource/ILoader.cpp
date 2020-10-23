/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-runtime-lib
 * Created on: 23 окт. 2020 г.
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

#include <lsp-plug.in/resource/ILoader.h>
#include <lsp-plug.in/io/InSequence.h>
#include <lsp-plug.in/io/InFileStream.h>

namespace lsp
{
    namespace resource
    {
        ILoader::ILoader()
        {
            nError          = STATUS_OK;
        }

        ILoader::~ILoader()
        {
            nError          = STATUS_OK;
        }

        io::IInStream *ILoader::read_stream(const char *name)
        {
            io::Path tmp;
            if ((nError = tmp.set(name)) != STATUS_OK)
                return NULL;
            return read_stream(&tmp);
        }

        io::IInStream *ILoader::read_stream(const LSPString *name)
        {
            io::Path tmp;
            if ((nError = tmp.set(name)) != STATUS_OK)
                return NULL;
            return read_stream(&tmp);
        }

        io::IInStream *ILoader::read_stream(const io::Path *name)
        {
            io::InFileStream *is = new io::InFileStream();
            if (is != NULL)
            {
                nError      = is->open(name);
                if (nError == STATUS_OK)
                    return is;
            }
            else
                nError      = STATUS_NO_MEM;

            if (is != NULL)
            {
                is->close();
                delete is;
            }

            return NULL;
        }

        io::IInSequence *ILoader::read_sequence(const char *name, const char *charset)
        {
            io::Path tmp;
            if ((nError = tmp.set(name)) != STATUS_OK)
                return NULL;
            return read_sequence(&tmp, charset);
        }

        io::IInSequence *ILoader::read_sequence(const LSPString *name, const char *charset)
        {
            io::Path tmp;
            if ((nError = tmp.set(name)) != STATUS_OK)
                return NULL;
            return read_sequence(&tmp, charset);
        }

        io::IInSequence *ILoader::read_sequence(const io::Path *name, const char *charset)
        {
            io::IInStream *is = read_stream(name);
            if (is == NULL)
                return NULL;

            io::InSequence *isq = new io::InSequence();
            if (isq != NULL)
            {
                // Wrap the file and return result
                nError = isq->wrap(is, WRAP_CLOSE | WRAP_DELETE, charset);
                if (nError == STATUS_OK)
                    return isq;
            }
            else
                nError = STATUS_NO_MEM;

            // Destroy allocated data
            if (isq != NULL)
            {
                isq->close();
                delete isq;
            }

            if (is != NULL)
            {
                is->close();
                delete is;
            }

            return NULL;
        }
    }
}
