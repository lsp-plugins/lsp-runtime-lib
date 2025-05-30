/*
 * Copyright (C) 2025 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2025 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-runtime-lib
 * Created on: 13 мар. 2019 г.
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

#include <lsp-plug.in/io/NativeFile.h>
#include <lsp-plug.in/io/StdioFile.h>
#include <lsp-plug.in/io/InFileStream.h>

namespace lsp
{
    namespace io
    {
        
        InFileStream::InFileStream()
        {
            pFD         = NULL;
            nWrapFlags  = 0;
        }
        
        InFileStream::~InFileStream()
        {
            // Close file descriptor
            if (pFD != NULL)
            {
                if (nWrapFlags & WRAP_CLOSE)
                    pFD->close();
                if (nWrapFlags & WRAP_DELETE)
                    delete pFD;
                pFD         = NULL;
            }
            nWrapFlags  = 0;
        }

        status_t InFileStream::close()
        {
            status_t res = STATUS_OK;

            // Close file descriptor
            if (pFD != NULL)
            {
                if (nWrapFlags & WRAP_CLOSE)
                    res = pFD->close();
                if (nWrapFlags & WRAP_DELETE)
                    delete pFD;
                pFD         = NULL;
            }
            nWrapFlags  = 0;

            return set_error(res);
        }

        status_t InFileStream::wrap(FILE *fd, bool close)
        {
            if (pFD != NULL)
                return set_error(STATUS_BAD_STATE);
            else if (fd == NULL)
                return set_error(STATUS_BAD_ARGUMENTS);

            StdioFile *f = new StdioFile();
            if (f == NULL)
                return set_error(STATUS_NO_MEM);
            status_t res = f->wrap(fd, File::FM_READ, close);
            if (res != STATUS_OK)
            {
                f->close();
                delete f;
                return set_error(res);
            }

            res = wrap(f, WRAP_DELETE);
            if (res != STATUS_OK)
            {
                f->close();
                delete f;
            }
            return set_error(res);
        }

        status_t InFileStream::wrap_native(fhandle_t fd, bool close)
        {
            if (pFD != NULL)
                return set_error(STATUS_BAD_STATE);

            NativeFile *f = new NativeFile();
            if (f == NULL)
                return set_error(STATUS_NO_MEM);
            status_t res = f->wrap(fd, File::FM_READ, close);
            if (res != STATUS_OK)
            {
                f->close();
                delete f;
                return set_error(res);
            }

            res = wrap(f, WRAP_DELETE);
            if (res != STATUS_OK)
            {
                f->close();
                delete f;
            }
            return set_error(res);
        }

        status_t InFileStream::wrap(File *fd, size_t flags)
        {
            if (pFD != NULL)
                return set_error(STATUS_BAD_STATE);
            else if (fd == NULL)
                return set_error(STATUS_BAD_ARGUMENTS);

            // Store pointers
            pFD         = fd;
            nWrapFlags  = flags;

            return set_error(STATUS_OK);
        }

        status_t InFileStream::open(const char *path)
        {
            if (pFD != NULL)
                return set_error(STATUS_BAD_STATE);
            else if (path == NULL)
                return set_error(STATUS_BAD_ARGUMENTS);

            LSPString tmp;
            if (!tmp.set_utf8(path))
                return set_error(STATUS_NO_MEM);
            return open(&tmp);
        }

        status_t InFileStream::open(const LSPString *path)
        {
            if (pFD != NULL)
                return set_error(STATUS_BAD_STATE);
            else if (path == NULL)
                return set_error(STATUS_BAD_ARGUMENTS);

            NativeFile *f = new NativeFile();
            if (f == NULL)
                return set_error(STATUS_NO_MEM);

            status_t res = f->open(path, File::FM_READ);
            if (res != STATUS_OK)
            {
                f->close();
                delete f;
                return set_error(res);
            }

            res = wrap(f, WRAP_CLOSE | WRAP_DELETE);
            if (res != STATUS_OK)
            {
                f->close();
                delete f;
            }

            return set_error(res);
        }

        status_t InFileStream::open(const Path *path)
        {
            return open(path->as_string());
        }

        wssize_t InFileStream::avail()
        {
            wssize_t pos = pFD->position();
            if (pos < 0)
            {
                set_error(status_t(-pos));
                return pos;
            }
            wssize_t size = pFD->size();
            if (size < 0)
            {
                set_error(status_t(-size));
                return size;
            }
            return size - pos;
        }

        wssize_t InFileStream::position()
        {
            if (pFD == NULL)
                return set_error(STATUS_CLOSED);
            wssize_t pos = pFD->position();
            set_error((pos >= 0) ? STATUS_OK : status_t(-pos));
            return pos;
        }

        ssize_t InFileStream::read(void *dst, size_t count)
        {
            if (pFD == NULL)
                return set_error(STATUS_CLOSED);
            ssize_t res = pFD->read(dst, count);
            set_error((res >= 0) ? STATUS_OK : status_t(-res));
            return res;
        }

        wssize_t InFileStream::seek(wsize_t position)
        {
            if (pFD == NULL)
                return set_error(STATUS_CLOSED);
            status_t res = pFD->seek(position, File::FSK_SET);
            if (res != STATUS_OK)
                return -set_error(res);
            wssize_t pos = pFD->position();
            set_error((pos >= 0) ? STATUS_OK : status_t(-pos));
            return pos;
        }

        wssize_t InFileStream::skip(wsize_t amount)
        {
            if (pFD == NULL)
                return set_error(STATUS_CLOSED);
            wssize_t before = pFD->position();
            if (before < 0)
                return IInStream::skip(amount);
            status_t res = pFD->seek(amount, File::FSK_CUR);
            if (res != STATUS_OK)
                return (res == STATUS_NOT_SUPPORTED) ?
                    IInStream::skip(amount) : set_error(res);

            wssize_t after = pFD->position();
            if (after < 0)
            {
                set_error(status_t(-after));
                return after;
            }

            return after - before;
        }


    
    } /* namespace io */
} /* namespace lsp */
