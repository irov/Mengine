#pragma once

#include "Config/Config.h"

#if defined(MENGINE_VENDOR_APPLE)
#   include <malloc/malloc.h>
#else
#   include <cstdlib>
#endif

namespace Mengine
{
    namespace StdLib
    {
        using std::malloc;
        using std::free;
        using std::calloc;
        using std::realloc;
    }
}

//////////////////////////////////////////////////////////////////////////
#ifndef MENGINE_MALLOC_SIZE
#   if defined(MENGINE_PLATFORM_WINDOWS)
#       define MENGINE_MALLOC_SIZE(p) ::_msize((p))
#   elif defined(MENGINE_VENDOR_APPLE)
#       define MENGINE_MALLOC_SIZE(p) ::malloc_size((p))
#   elif defined(MENGINE_PLATFORM_ANDROID)
#       define MENGINE_MALLOC_SIZE(p) ::malloc_usable_size((p))
#   endif
#endif
//////////////////////////////////////////////////////////////////////////
