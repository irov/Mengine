#pragma once

#if defined(MENGINE_DEBUG)
#include "Kernel/Identity.h"
#endif

namespace Mengine
{
#if defined(MENGINE_DEBUG)
#define MENGINE_MIXIN_DEBUG_NAME(Mixin) (dynamic_cast<const Identity *>(Mixin)->getName().c_str())
#define MENGINE_MIXIN_DEBUG_TYPE(Mixin) (dynamic_cast<const Identity *>(Mixin)->getType().c_str())
#else
#define MENGINE_MIXIN_DEBUG_NAME(Mixin) ("UNKNOWN")
#define MENGINE_MIXIN_DEBUG_TYPE(Mixin) ("UNKNOWN")
#endif
}