#pragma once

#include "Config/Config.h"

#include <cstring>
#include <cwchar>

namespace Mengine
{
    namespace StdString
    {
        using std::strlen;
        using std::strcmp;
        using std::strcpy;
        using std::strncpy;
        using std::strcat;
        using std::strncat;
        using std::strchr;
        using std::strrchr;
        using std::strncmp;
        using std::strstr;

        using std::wcslen;
        using std::wcscmp;
        using std::wcschr;
        using std::wcscpy;
        using std::wcsncpy;
        using std::wcscat;
        using std::wcsncat;
        using std::wcsrchr;
        using std::wcsncmp;

        using std::memcpy;
        using std::memset;
        using std::memmove;
        using std::memcmp;
    }
}

//////////////////////////////////////////////////////////////////////////
#ifndef MENGINE_STRNCPY_STATIC
#define MENGINE_STRNCPY_STATIC(a, b, n) (Mengine::StdString::strncpy((a), (b), (n)), MENGINE_STATIC_STRING_LENGTH(b))
#endif
//////////////////////////////////////////////////////////////////////////
#ifndef MENGINE_WCSICMP
#   if defined(MENGINE_PLATFORM_WINDOWS)
#       define MENGINE_WCSICMP(a,b) ::_wcsicmp((a), (b))
#   endif
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
