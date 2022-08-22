#pragma once

#include "Config/Config.h"

#ifdef MENGINE_PLATFORM_APPLE
#   include <malloc/malloc.h>
#else
#   include <cstdlib>
#endif

//////////////////////////////////////////////////////////////////////////
#ifndef MENGINE_MALLOC
#define MENGINE_MALLOC(s) ::malloc((s))
#endif
//////////////////////////////////////////////////////////////////////////
#ifndef MENGINE_FREE
#define MENGINE_FREE(p) ::free((p))
#endif
//////////////////////////////////////////////////////////////////////////
#ifndef MENGINE_CALLOC
#define MENGINE_CALLOC(c, s) ::calloc((c), (s))
#endif
//////////////////////////////////////////////////////////////////////////
#ifndef MENGINE_REALLOC
#define MENGINE_REALLOC(p, s) ::realloc((p), (s))
#endif
//////////////////////////////////////////////////////////////////////////
#ifndef MENGINE_MALLOC_SIZE
#   if defined(MENGINE_PLATFORM_WINDOWS)
#       define MENGINE_MALLOC_SIZE(p) ::_msize((p))
#   elif defined(MENGINE_PLATFORM_APPLE)
#       define MENGINE_MALLOC_SIZE(p) ::malloc_size((p))
#   elif defined(MENGINE_PLATFORM_ANDROID)
#       define MENGINE_MALLOC_SIZE(p) ::malloc_usable_size((p))
#   endif
#endif
//////////////////////////////////////////////////////////////////////////
