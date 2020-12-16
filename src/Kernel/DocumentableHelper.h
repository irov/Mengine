#pragma once

#include "Kernel/Documentable.h"

//////////////////////////////////////////////////////////////////////////
#if MENGINE_DOCUMENT_ENABLE
#   define MENGINE_DOCUMENTABLE_STR(Ptr, Empty) (dynamic_cast<const Mengine::Documentable*>(Ptr)->getDocument()->getMessage())
#else
#   define MENGINE_DOCUMENTABLE_STR(Ptr, Empty) (Empty)
#endif
//////////////////////////////////////////////////////////////////////////