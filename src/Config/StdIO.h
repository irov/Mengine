#pragma once

#include <stdio.h>
#include <stdarg.h>

#ifdef MENGINE_TOOLCHAIN_MSVC
#define MENGINE_VSNPRINTF(Buffer, Capacity, Format, Args) ::vsnprintf( Buffer, Capacity, Format, Args )
#else
#define MENGINE_VSNPRINTF(Buffer, Capacity, Format, Args) ::vsprintf( Buffer, Format, Args )
#endif