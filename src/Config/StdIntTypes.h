#pragma once

#include "Config/Config.h"

#define __STDC_FORMAT_MACROS
#include <cinttypes>

#ifndef MENGINE_PRId8
#define MENGINE_PRId8 PRId8
#endif

#ifndef MENGINE_PRId16
#define MENGINE_PRId16 PRId16
#endif

#ifndef MENGINE_PRId32
#define MENGINE_PRId32 PRId32
#endif

#ifndef MENGINE_PRId64
#define MENGINE_PRId64 PRId64
#endif

#ifndef MENGINE_PRIu8
#define MENGINE_PRIu8 PRIu8
#endif

#ifndef MENGINE_PRIu16
#define MENGINE_PRIu16 PRIu16
#endif

#ifndef MENGINE_PRIu32
#define MENGINE_PRIu32 PRIu32
#endif

#ifndef MENGINE_PRIu64
#define MENGINE_PRIu64 PRIu64
#endif

#ifndef MENGINE_PRIuPTR
#define MENGINE_PRIuPTR PRIuPTR
#endif

#ifndef MENGINE_PRDWORD
#   if defined(MENGINE_PLATFORM_WINDOWS64)
#       define MENGINE_PRDWORD "lu"
#   else
#       define MENGINE_PRDWORD "lu"
#   endif
#endif

#ifndef MENGINE_PRDWORD_PTR
#   if defined(MENGINE_PLATFORM_WINDOWS64)
#       define MENGINE_PRDWORD_PTR "llu"
#   else
#       define MENGINE_PRDWORD_PTR "lu"
#   endif
#endif

#ifndef MENGINE_PRWPARAM
#   if defined(MENGINE_PLATFORM_WINDOWS64)
#       define MENGINE_PRWPARAM "llu"
#   else
#       define MENGINE_PRWPARAM "u"
#   endif
#endif

#ifndef MENGINE_PRLPARAM
#   if defined(MENGINE_PLATFORM_WINDOWS64)
#       define MENGINE_PRLPARAM "lld"
#   else
#       define MENGINE_PRLPARAM "ld"
#   endif
#endif

#ifndef MENGINE_SCNu8
#define MENGINE_SCNu8 SCNu8
#endif

#ifndef MENGINE_SCNu16
#define MENGINE_SCNu16 SCNu16
#endif

#ifndef MENGINE_SCNu32
#define MENGINE_SCNu32 SCNu32
#endif

#ifndef MENGINE_SCNu64
#define MENGINE_SCNu64 SCNu64
#endif

#ifndef MENGINE_SCNd8
#define MENGINE_SCNd8 SCNd8
#endif

#ifndef MENGINE_SCNd16
#define MENGINE_SCNd16 SCNd16
#endif

#ifndef MENGINE_SCNd32
#define MENGINE_SCNd32 SCNd32
#endif

#ifndef MENGINE_SCNd64
#define MENGINE_SCNd64 SCNd64
#endif