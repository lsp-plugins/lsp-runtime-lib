/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-runtime-lib
 * Created on: 9 апр. 2020 г.
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

#ifndef LSP_PLUG_IN_RUNTIME_VERSION_H_
#define LSP_PLUG_IN_RUNTIME_VERSION_H_

#define LSP_RUNTIME_LIB_MAJOR       1
#define LSP_RUNTIME_LIB_MINOR       0
#define LSP_RUNTIME_LIB_MICRO       4

#ifdef LSP_RUNTIME_LIB_BUILTIN
    #define LSP_RUNTIME_LIB_EXPORT
    #define LSP_RUNTIME_LIB_CEXPORT
    #define LSP_RUNTIME_LIB_IMPORT          LSP_SYMBOL_IMPORT
    #define LSP_RUNTIME_LIB_CIMPORT         LSP_CSYMBOL_IMPORT
#else
    #define LSP_RUNTIME_LIB_EXPORT          LSP_SYMBOL_EXPORT
    #define LSP_RUNTIME_LIB_CEXPORT         LSP_CSYMBOL_EXPORT
    #define LSP_RUNTIME_LIB_IMPORT          LSP_SYMBOL_IMPORT
    #define LSP_RUNTIME_LIB_CIMPORT         LSP_CSYMBOL_IMPORT
#endif

#endif /* LSP_PLUG_IN_RUNTIME_VERSION_H_ */
