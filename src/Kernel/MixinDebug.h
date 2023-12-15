#pragma once

#include "Config/UniqueId.h"

#if defined(MENGINE_DEBUG)
#   include "Kernel/Identity.h"
#   include "Kernel/Factorable.h"
#   if defined(MENGINE_DOCUMENT_ENABLE)
#       include "Kernel/Documentable.h"
#   endif

#   include "Config/Typeinfo.h"
#endif

namespace Mengine
{
#if defined(MENGINE_DEBUG)
#   define MENGINE_MIXIN_DEBUG_NAME(Object) (dynamic_cast<const Mengine::Identity *>(Object) != nullptr ? dynamic_cast<const Mengine::Identity *>(Object)->getName().c_str() : "UNKNOWN")
#   define MENGINE_MIXIN_DEBUG_TYPE(Object) (dynamic_cast<const Mengine::Factorable *>(Object) != nullptr ? dynamic_cast<const Mengine::Factorable *>(Object)->getType().c_str() : MENGINE_TYPEINFO_NAME(Object))
#   define MENGINE_MIXIN_DEBUG_UID(Object) (dynamic_cast<const Mengine::Identity *>(Object) != nullptr ? dynamic_cast<const Mengine::Identity *>(Object)->getUniqueIdentity() : Mengine::INVALID_UNIQUE_ID)
#   if defined(MENGINE_DOCUMENT_ENABLE)
#       define MENGINE_MIXIN_DEBUG_DOCUMENT(Object) (dynamic_cast<const Mengine::Documentable *>(Object) != nullptr ? dynamic_cast<const Mengine::Documentable *>(Object)->getDocument() : nullptr)
#   else
#       define MENGINE_MIXIN_DEBUG_DOCUMENT(Object) (Mengine::DocumentablePtr::none())
#   endif
#else
#   define MENGINE_MIXIN_DEBUG_NAME(Mixin) ("UNKNOWN")
#   define MENGINE_MIXIN_DEBUG_TYPE(Mixin) ("UNKNOWN")
#   define MENGINE_MIXIN_DEBUG_UID(Mixin) (Mengine::INVALID_UNIQUE_ID)
#   define MENGINE_MIXIN_DEBUG_DOCUMENT(Mixin) (Mengine::DocumentablePtr::none())
#endif
}
