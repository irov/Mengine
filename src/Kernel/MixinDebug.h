#pragma once

#include "Config/Config.h"
#include "Config/UniqueId.h"

#if defined(MENGINE_DEBUG)
#   include "Kernel/Identity.h"
#endif

namespace Mengine
{    
#if defined(MENGINE_DEBUG)
#   define MENGINE_MIXIN_DEBUG_NAME(Mixin) (dynamic_cast<const Mengine::Identity *>(Mixin) != nullptr ? dynamic_cast<const Mengine::Identity *>(Mixin)->getName().c_str() : "UNKNOWN")
#   define MENGINE_MIXIN_DEBUG_TYPE(Mixin) (dynamic_cast<const Mengine::Identity *>(Mixin) != nullptr ? dynamic_cast<const Mengine::Identity *>(Mixin)->getType().c_str() : "UNKNOWN")
#   define MENGINE_MIXIN_DEBUG_UID(Mixin) (dynamic_cast<const Mengine::Identity *>(Mixin) != nullptr ? dynamic_cast<const Mengine::Identity *>(Mixin)->getUniqueIdentity() : Mengine::INVALIDATE_UNIQUE_ID)
#else
#   define MENGINE_MIXIN_DEBUG_NAME(Mixin) ("UNKNOWN")
#   define MENGINE_MIXIN_DEBUG_TYPE(Mixin) ("UNKNOWN")
#   define MENGINE_MIXIN_DEBUG_UID(Mixin) (Mengine::INVALIDATE_UNIQUE_ID)
#endif
}