/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-runtime-lib
 * Created on: 30 авг. 2017 г.
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

#ifndef LSP_PLUG_IN_RUNTIME_STATIC_H_
#define LSP_PLUG_IN_RUNTIME_STATIC_H_

#include <lsp-plug.in/runtime/version.h>
#include <lsp-plug.in/common/types.h>

namespace lsp
{
    /**
     * Special object that triggers routine on startup
     */
    class StaticInitializer
    {
        public:
            typedef void (* initializer_t)();

        public:
            StaticInitializer(initializer_t init)
            {
                init();
            }

            ~StaticInitializer()
            {
            }
    };

    /**
     * Special object that triggers routine on finalization
     */
    class StaticFinalizer
    {
        public:
            typedef void (* finalizer_t)();

        private:
            finalizer_t     pFini;      // Finalization routine

        public:
            StaticFinalizer(finalizer_t fini)
            {
                this->pFini = fini;
            }

            ~StaticFinalizer()
            {
                if (pFini != NULL)
                {
                    pFini();
                    pFini   = NULL;
                }
            }
    };

    /**
     * Special object that triggers routine on startup and finalization
     */
    class StaticManager
    {
        public:
            typedef void (* initializer_t)();
            typedef void (* finalizer_t)();

        private:
            finalizer_t     pFini;      // Finalization routine

        public:
            StaticManager(initializer_t init, finalizer_t fini)
            {
                pFini = fini;
                if (init != NULL)
                    init();
            }

            ~StaticManager()
            {
                if (pFini != NULL)
                {
                    pFini();
                    pFini   = NULL;
                }
            }
    };
}

#endif /* LSP_PLUG_IN_RUNTIME_STATIC_H_ */
