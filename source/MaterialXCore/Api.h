//
// TM & (c) 2017 Lucasfilm Entertainment Company Ltd. and Lucasfilm Ltd.
// All rights reserved.  See LICENSE.txt for license.
//

#ifndef MATERIALX_CORE_API_H
#define MATERIALX_CORE_API_H

#include <MaterialXCore/Platform.h>

/// @file
/// Import and export declarations for the Core library.

#if defined(MATERIALX_CORE_EXPORTS)
    #define MX_CORE_API MATERIALX_SYMBOL_EXPORT
    #define MX_CORE_EXTERN_TEMPLATE(...) MATERIALX_EXPORT_EXTERN_TEMPLATE(__VA_ARGS__)
#else
    #define MX_CORE_API MATERIALX_SYMBOL_IMPORT
    #define MX_CORE_EXTERN_TEMPLATE(...) MATERIALX_IMPORT_EXTERN_TEMPLATE(__VA_ARGS__)
#endif

#endif
