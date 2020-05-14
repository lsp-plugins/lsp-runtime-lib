/*
 * version.h
 *
 *  Created on: 9 апр. 2020 г.
 *      Author: sadko
 */

#ifndef LSP_PLUG_IN_RUNTIME_VERSION_H_
#define LSP_PLUG_IN_RUNTIME_VERSION_H_

#define LSP_RUNTIME_LIB_MAJOR       0
#define LSP_RUNTIME_LIB_MINOR       5
#define LSP_RUNTIME_LIB_MICRO       2

#ifdef LSP_RUNTIME_LIB_BUILTIN
    #define LSP_RUNTIME_LIB_EXPORT
    #define LSP_RUNTIME_LIB_CEXPORT
    #define LSP_RUNTIME_LIB_IMPORT
    #define LSP_RUNTIME_LIB_CIMPORT
#else
    #define LSP_RUNTIME_LIB_EXPORT          LSP_SYMBOL_EXPORT
    #define LSP_RUNTIME_LIB_CEXPORT         LSP_CSYMBOL_EXPORT
    #define LSP_RUNTIME_LIB_IMPORT          LSP_SYMBOL_IMPORT
    #define LSP_RUNTIME_LIB_CIMPORT         LSP_CSYMBOL_IMPORT
#endif

#endif /* LSP_PLUG_IN_RUNTIME_VERSION_H_ */
