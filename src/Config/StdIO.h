#pragma once

#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#define __STDC_FORMAT_MACROS

#include <inttypes.h>

#define MENGINE_VA_LIST_TYPE va_list
#define MENGINE_VA_LIST_START( Args, Format ) va_start( Args, Format )
#define MENGINE_VA_LIST_END( Args ) va_end( Args )

#ifdef MENGINE_TOOLCHAIN_MSVC
    #define MENGINE_VSNPRINTF(Buffer, Capacity, Format, Args) ::vsnprintf( Buffer, Capacity, Format, Args )
#else
    #define MENGINE_VSNPRINTF(Buffer, Capacity, Format, Args) ::vsprintf( Buffer, Format, Args )
#endif

#if defined(MENGINE_COMPILER_MSVC)
#   define MENGINE_STRICMP _stricmp
#elif defined(MENGINE_PLATFORM_LINUX) && defined(MENGINE_COMPILER_GCC)
#   define MENGINE_STRICMP strcasecmp
#else
#   define MENGINE_STRICMP stricmp
#endif

#define MENGINE_SSCANF(Buffer, ...) ::sscanf( Buffer, __VA_ARGS__ )
#define MENGINE_SNPRINTF(Buffer, Capacity, ...) ::snprintf( Buffer, Capacity, __VA_ARGS__ )