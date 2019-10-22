#pragma once

#include <stdio.h>
#include <stdarg.h>

#define MENGINE_VA_LIST_TYPE va_list
#define MENGINE_VA_LIST_START( Args, Format ) va_start( Args, Format )
#define MENGINE_VA_LIST_END( Args ) va_end( Args )

#ifdef MENGINE_TOOLCHAIN_MSVC
#define MENGINE_VSNPRINTF(Buffer, Capacity, Format, Args) ::vsnprintf( Buffer, Capacity, Format, Args )
#else
#define MENGINE_VSNPRINTF(Buffer, Capacity, Format, Args) ::vsprintf( Buffer, Format, Args )
#endif