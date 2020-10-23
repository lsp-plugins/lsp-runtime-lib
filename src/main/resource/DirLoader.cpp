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

#include <lsp-plug.in/resource/DirLoader.h>

namespace lsp
{
    namespace resource
    {
        DirLoader::DirLoader()
        {
            bEnforce        = false;
        }

        DirLoader::~DirLoader()
        {
        }

        status_t DirLoader::set_path(const char *path)
        {
            return sPath.set(path);
        }

        status_t DirLoader::set_path(const LSPString *path)
        {
            return sPath.set(path);
        }

        status_t DirLoader::set_path(const io::Path *path)
        {
            return sPath.set(path);
        }

        bool DirLoader::set_enforce(bool enforce)
        {
            bool old = bEnforce;
            bEnforce = enforce;
            return old;
        }

        io::IInStream *DirLoader::read_stream(const io::Path *name)
        {
            if (!bEnforce)
                return ILoader::read_stream(name);

            io::Path tmp;
            nError  = tmp.set(name);
            if (nError == STATUS_OK)
                nError  = tmp.canonicalize();
            if (nError == STATUS_OK)
                nError  = tmp.remove_root();
            if (nError == STATUS_OK)
                nError  = tmp.set_parent(&sPath);

            return (nError == STATUS_OK) ? ILoader::read_stream(&tmp) : NULL;
        }
    }
}


