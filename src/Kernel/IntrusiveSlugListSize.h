#pragma once

#include "Kernel/IntrusivePtr.h"
#include "Kernel/IntrusivePtrBase.h"
#include "Kernel/IntrusiveSlugHead.h"

#include "stdex/intrusive_slug_list_size_ptr.h"

namespace Mengine
{ 
    template<class T>
    class IntrusiveSlugListSize
        : public stdex::intrusive_slug_list_size_ptr<T, IntrusivePtr, IntrusivePtrBase, IntrusiveSlugHead>
    {
    };
}