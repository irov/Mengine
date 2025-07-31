#pragma once

#include "Kernel/Documentable.h"

#include "Config/UniqueId.h"

#if defined(MENGINE_DEBUG)
#   include "Kernel/Identity.h"
#   include "Kernel/Factorable.h"

#   include "Config/Typeinfo.h"
#   include "Config/DynamicCast.h"

#   define MENGINE_MIXIN_DEBUG_NAME(Object) (Helper::dynamicCast<const Mengine::Identity *>(Object) != nullptr ? Helper::dynamicCast<const Mengine::Identity *>(Object)->getName().c_str() : "UNKNOWN")
#   define MENGINE_MIXIN_DEBUG_TYPE(Object) (Helper::dynamicCast<const Mengine::Factorable *>(Object) != nullptr ? Helper::dynamicCast<const Mengine::Factorable *>(Object)->getType().c_str() : MENGINE_TYPEINFO_NAME(Object))
#   define MENGINE_MIXIN_DEBUG_UID(Object) (Helper::dynamicCast<const Mengine::Factorable *>(Object) != nullptr ? Helper::dynamicCast<const Mengine::Factorable *>(Object)->getUniqueIdentity() : Mengine::INVALID_UNIQUE_ID)

#   if defined(MENGINE_DOCUMENT_ENABLE)
#       define MENGINE_MIXIN_DEBUG_DOCUMENT(Object) (Helper::dynamicCast<const Mengine::Documentable *>(Object) != nullptr ? Helper::dynamicCast<const Mengine::Documentable *>(Object)->getDocument() : nullptr)
#   else
#       define MENGINE_MIXIN_DEBUG_DOCUMENT(Object) (Mengine::DocumentablePtr::none())
#   endif
#else
#   define MENGINE_MIXIN_DEBUG_NAME(Mixin) ("UNKNOWN")
#   define MENGINE_MIXIN_DEBUG_TYPE(Mixin) ("UNKNOWN")
#   define MENGINE_MIXIN_DEBUG_UID(Mixin) (Mengine::INVALID_UNIQUE_ID)
#   define MENGINE_MIXIN_DEBUG_DOCUMENT(Mixin) (Mengine::DocumentablePtr::none())
#endif
