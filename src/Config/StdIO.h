#pragma once

#include "Config/Config.h"

#include <cstdio>

#ifndef MENGINE_VSNPRINTF
#if defined(MENGINE_TOOLCHAIN_MSVC)
#   define MENGINE_VSNPRINTF(Buffer, Capacity, Format, Args) ::vsnprintf( Buffer, Capacity, Format, Args )
#elif defined(MENGINE_TOOLCHAIN_MINGW)
#   define MENGINE_VSNPRINTF(Buffer, Capacity, Format, Args) ::vsnprintf( Buffer, Capacity, Format, Args )
#else
#   define MENGINE_VSNPRINTF(Buffer, Capacity, Format, Args) ::vsprintf( Buffer, Format, Args )
#endif
#endif

#ifndef MENGINE_SSCANF
#define MENGINE_SSCANF(Buffer, ...) ::sscanf( Buffer, __VA_ARGS__ )
#endif

#ifndef MENGINE_SPRINTF
#define MENGINE_SPRINTF(Buffer, ...) ::sprintf(Buffer, __VA_ARGS__ )
#endif

#ifndef MENGINE_SNPRINTF
#define MENGINE_SNPRINTF(Buffer, Capacity, ...) ::snprintf( Buffer, Capacity, __VA_ARGS__ )
#endif

#ifndef MENGINE_VSPRINTF
#define MENGINE_VSPRINTF(Buffer, Format, Args) ::vsprintf( Buffer, Format, Args )
#endif