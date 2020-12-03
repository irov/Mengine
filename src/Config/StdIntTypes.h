#pragma once

#include "Config/Config.h"

#define __STDC_FORMAT_MACROS
#include <cinttypes>

#ifdef MENGINE_PLATFORM_WINDOWS64
#define MENGINE_PRDWORD "lu"
#else
#define MENGINE_PRDWORD "lu"
#endif

#ifdef MENGINE_PLATFORM_WINDOWS64
#define MENGINE_PRDWORD_PTR "llu"
#else
#define MENGINE_PRDWORD_PTR "lu"
#endif

#ifdef MENGINE_PLATFORM_WINDOWS64
#define MENGINE_PRWPARAM "llu"
#else
#define MENGINE_PRWPARAM "u"
#endif

#ifdef MENGINE_PLATFORM_WINDOWS64
#define MENGINE_PRLPARAM "lld"
#else
#define MENGINE_PRLPARAM "ld"
#endif