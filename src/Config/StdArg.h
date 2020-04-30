#pragma once

#include <cstdarg>

#ifndef MENGINE_VA_LIST_TYPE
#define MENGINE_VA_LIST_TYPE va_list
#endif

#ifndef MENGINE_VA_LIST_START
#define MENGINE_VA_LIST_START( Args, Format ) va_start( Args, Format )
#endif

#ifndef MENGINE_VA_LIST_END
#define MENGINE_VA_LIST_END( Args ) va_end( Args )
#endif