#pragma once

#include "Config/Config.h"

#include <string.h>
#include <wchar.h>

//////////////////////////////////////////////////////////////////////////
#ifndef MENGINE_MEMCPY
#define MENGINE_MEMCPY(a, b, n) ::memcpy((a), (b), (n))
#endif
//////////////////////////////////////////////////////////////////////////
#ifndef MENGINE_MEMSET
#define MENGINE_MEMSET(d, v, s) ::memset((d), (v), (s))
#endif
//////////////////////////////////////////////////////////////////////////
#ifndef MENGINE_MEMMOVE
#define MENGINE_MEMMOVE(d, v, s) ::memmove((d), (v), (s))
#endif
//////////////////////////////////////////////////////////////////////////
#ifndef MENGINE_MEMCMP
#define MENGINE_MEMCMP(a, b, n) ::memcmp((a), (b), (n))
#endif
//////////////////////////////////////////////////////////////////////////
#ifndef MENGINE_STRICMP
#if defined(MENGINE_COMPILER_MSVC)
#   define MENGINE_STRICMP(a, b) ::_stricmp((a), (b))
#elif defined(MENGINE_PLATFORM_LINUX) && defined(MENGINE_COMPILER_GCC)
#   define MENGINE_STRICMP(a, b) ::strcasecmp((a), (b))
#elif defined(MENGINE_PLATFORM_ANDROID)
#   define MENGINE_STRICMP(a, b) ::strcasecmp((a), (b))
#elif defined(MENGINE_PLATFORM_MACOS)
#   define MENGINE_STRICMP(a, b) ::strcasecmp((a), (b))
#elif defined(MENGINE_PLATFORM_IOS)
#   define MENGINE_STRICMP(a, b) ::strcasecmp((a), (b))
#else
#   define MENGINE_STRICMP(a, b) ::stricmp((a), (b))
#endif
#endif
//////////////////////////////////////////////////////////////////////////
