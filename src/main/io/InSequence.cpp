/*
 * Copyright (C) 2025 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2025 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-runtime-lib
 * Created on: 16 июн. 2018 г.
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

#include <lsp-plug.in/io/charset.h>
#include <lsp-plug.in/io/StdioFile.h>
#include <lsp-plug.in/io/NativeFile.h>
#include <lsp-plug.in/io/InFileStream.h>
#include <lsp-plug.in/io/InSequence.h>

#include <errno.h>

#define CBUF_SIZE        0x1000
#define BBUF_SIZE        0x4000

namespace lsp
{
    namespace io
    {
        InSequence::InSequence()
        {
            pIS         = NULL;
            nWrapFlags  = 0;
        }

        InSequence::~InSequence()
        {
            // Close file descriptor
            if (pIS != NULL)
            {
                if (nWrapFlags & WRAP_CLOSE)
                    pIS->close();
                if (nWrapFlags & WRAP_DELETE)
                    delete pIS;
                pIS         = NULL;
            }
            nWrapFlags  = 0;

            // Close decoder
            sDecoder.close();
        }

        status_t InSequence::close()
        {
            status_t res = STATUS_OK;

            // Close file descriptor
            if (pIS != NULL)
            {
                if (nWrapFlags & WRAP_CLOSE)
                    res = pIS->close();
                if (nWrapFlags & WRAP_DELETE)
                    delete pIS;
                pIS         = NULL;
            }
            nWrapFlags  = 0;

            // Close decoder
            sDecoder.close();

            // Return result
            return set_error(res);
        }
    
        status_t InSequence::wrap(FILE *fd, bool close, const char *charset)
        {
            if (pIS != NULL)
                return set_error(STATUS_BAD_STATE);
            else if (fd == NULL)
                return set_error(STATUS_BAD_ARGUMENTS);

            // Create input file stream
            InFileStream *is = new InFileStream();
            status_t res = is->wrap(fd, close);
            if (res != STATUS_OK)
            {
                is->close();
                delete is;
                return set_error(res);
            }

            // Wrap input file stream
            res = wrap(is, WRAP_CLOSE | WRAP_DELETE, charset);
            if (res != STATUS_OK)
            {
                is->close();
                delete is;
                return set_error(res);
            }

            return set_error(res);
        }

        status_t InSequence::wrap_native(fhandle_t fd, bool close, const char *charset)
        {
            if (pIS != NULL)
                return set_error(STATUS_BAD_STATE);

            // Create input file stream
            InFileStream *is = new InFileStream();
            status_t res = is->wrap_native(fd, close);
            if (res != STATUS_OK)
            {
                is->close();
                delete is;
                return set_error(res);
            }

            // Wrap input file stream
            res = wrap(is, WRAP_CLOSE | WRAP_DELETE, charset);
            if (res != STATUS_OK)
            {
                is->close();
                delete is;
                return set_error(res);
            }

            return set_error(res);
        }

        status_t InSequence::wrap(File *fd, size_t flags, const char *charset)
        {
            if (pIS != NULL)
                return set_error(STATUS_BAD_STATE);
            else if (fd == NULL)
                return set_error(STATUS_BAD_ARGUMENTS);

            // Create input file stream
            InFileStream *is = new InFileStream();
            status_t res = is->wrap(fd, flags);
            if (res != STATUS_OK)
            {
                is->close();
                delete is;
                return set_error(res);
            }

            // Wrap input file stream
            res = wrap(is, WRAP_CLOSE | WRAP_DELETE, charset);
            if (res != STATUS_OK)
            {
                is->close();
                delete is;
                return set_error(res);
            }

            return set_error(res);
        }

        status_t InSequence::wrap(IInStream *is, size_t flags, const char *charset)
        {
            if (pIS != NULL)
                return set_error(STATUS_BAD_STATE);
            else if (is == NULL)
                return set_error(STATUS_BAD_ARGUMENTS);

            // Initialize decoder
            status_t res = sDecoder.init(charset);
            if (res != STATUS_OK)
            {
                sDecoder.close();
                return set_error(res);
            }

            // Store pointers
            pIS         = is;
            nWrapFlags  = flags;

            return set_error(STATUS_OK);
        }

        status_t InSequence::open(const char *path, const char *charset)
        {
            if (pIS != NULL)
                return set_error(STATUS_BAD_STATE);
            else if (path == NULL)
                return set_error(STATUS_BAD_ARGUMENTS);

            LSPString tmp;
            if (!tmp.set_utf8(path))
                return set_error(STATUS_NO_MEM);
            return open(&tmp, charset);
        }

        status_t InSequence::open(const LSPString *path, const char *charset)
        {
            if (pIS != NULL)
                return set_error(STATUS_BAD_STATE);
            else if (path == NULL)
                return set_error(STATUS_BAD_ARGUMENTS);

            // Create input file stream
            InFileStream *is = new InFileStream();
            status_t res = is->open(path);
            if (res != STATUS_OK)
            {
                is->close();
                delete is;
                return set_error(res);
            }

            // Wrap input file stream
            res = wrap(is, WRAP_CLOSE | WRAP_DELETE, charset);
            if (res != STATUS_OK)
            {
                is->close();
                delete is;
                return set_error(res);
            }

            return set_error(res);
        }

        status_t InSequence::open(const Path *path, const char *charset)
        {
            return open(path->as_string(), charset);
        }

        ssize_t InSequence::read(lsp_wchar_t *dst, size_t count)
        {
            if (pIS == NULL)
                return -set_error(STATUS_CLOSED);

            // Clear line buffer
            sLine.clear();

            size_t n_read = 0;
            while (n_read < count)
            {
                // Try to fetch data
                ssize_t fetched = sDecoder.fetch(dst, count - n_read);
                if (fetched > 0)
                {
                    n_read     += fetched;
                    dst        += fetched;
                    continue;
                }

                // No data to fetch? Try to fill buffer
                ssize_t filled  = sDecoder.fill(pIS);
                if (filled > 0)
                    continue;

                // Nothing to do more? Skip any errors if there was data processed
                if (n_read > 0)
                    break;

                // Analyze errors
                if (fetched < 0)
                    return -set_error(status_t(-fetched));
                else if (filled < 0)
                    return -set_error(status_t(-filled));

                set_error(STATUS_OK);
                break;
            }

            return n_read;
        }

        lsp_swchar_t InSequence::read_internal()
        {
            // Try to fetch character
            lsp_swchar_t ch = sDecoder.fetch();
            if (ch < 0)
            {
                // Analyze error
                if (ch != -STATUS_EOF)
                    return -set_error(-ch);

                // No data to fetch? Try to fill buffer
                ssize_t filled  = sDecoder.fill(pIS);
                if (filled < 0)
                    return -set_error(status_t(-filled));
                else if (filled == 0)
                    return -set_error(STATUS_EOF);

                // Try to fetch character again
                ch  = sDecoder.fetch();
                if (ch < 0)
                    return -set_error(-ch);
            }
            return ch;
        }

        lsp_swchar_t InSequence::read()
        {
            if (pIS == NULL)
                return -set_error(STATUS_CLOSED);

            // Clear line buffer
            sLine.clear();
            return read_internal();
        }

        status_t InSequence::read_line(LSPString *s, bool force)
        {
            if (pIS == NULL)
                return set_error(STATUS_CLOSED);

            while (true)
            {
                // Try to fetch character
                lsp_swchar_t ch = read_internal();
                if (ch < 0)
                {
                    if (ch == -STATUS_EOF)
                        break;
                    return set_error(-ch);
                }

                // End of line?
                if (ch == '\n')
                {
                    if (sLine.last() == '\r')
                        sLine.remove_last();
                    s->take(&sLine);
                    return set_error(STATUS_OK);
                }

                // Append character
                if (!sLine.append(lsp_wchar_t(ch)))
                    return set_error(STATUS_NO_MEM);
            }

            // Check force flag
            if ((force) && (sLine.length() > 0))
            {
                s->take(&sLine);
                return set_error(STATUS_OK);
            }

            return set_error(STATUS_EOF);
        }

        ssize_t InSequence::skip(size_t count)
        {
            sLine.clear();
            return IInSequence::skip(count);
        }

    } /* namespace io */
} /* namespace lsp */
