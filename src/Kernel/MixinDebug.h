#pragma once

#if defined(MENGINE_DEBUG)
#   include "Kernel/Identity.h"
#   include "Kernel/Unique.h"
#endif

namespace Mengine
{
#if defined(MENGINE_DEBUG)
#   define MENGINE_MIXIN_DEBUG_NAME(Mixin) (dynamic_cast<const Identity *>(Mixin) != nullptr ? dynamic_cast<const Identity *>(Mixin)->getName().c_str() : "UNKNOWN")
#   define MENGINE_MIXIN_DEBUG_TYPE(Mixin) (dynamic_cast<const Identity *>(Mixin) != nullptr ? dynamic_cast<const Identity *>(Mixin)->getType().c_str() : "UNKNOWN")
#   define MENGINE_MIXIN_DEBUG_UID(Mixin) (dynamic_cast<const Unique *>(Mixin) != nullptr ? dynamic_cast<const Unique *>(Mixin)->getUniqueIdentity() : 0)
#else
#   define MENGINE_MIXIN_DEBUG_NAME(Mixin) ("UNKNOWN")
#   define MENGINE_MIXIN_DEBUG_TYPE(Mixin) ("UNKNOWN")
#   define MENGINE_MIXIN_DEBUG_UID(Mixin) (0)
#endif
}