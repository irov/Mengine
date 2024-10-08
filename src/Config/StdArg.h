#pragma once

#include "Config/Config.h"

#include <cstdarg>

#ifndef MENGINE_VA_LIST_TYPE
#define MENGINE_VA_LIST_TYPE std::va_list
#endif

#ifndef MENGINE_VA_LIST_START
#define MENGINE_VA_LIST_START( Args, Base ) va_start( Args, Base )
#endif

#ifndef MENGINE_VA_LIST_END
#define MENGINE_VA_LIST_END( Args ) va_end( Args )
#endif

#ifndef MENGINE_VA_LIST_COPY
#define MENGINE_VA_LIST_COPY( Dst, Src ) va_copy( Dst, Src )
#endif