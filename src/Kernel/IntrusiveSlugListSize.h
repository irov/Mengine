#pragma once

#include "Kernel/IntrusivePtr.h"
#include "Kernel/IntrusivePtrBase.h"
#include "Kernel/IntrusiveSlugHead.h"

#include "stdex/intrusive_slug_list_size_ptr.h"

namespace Mengine
{
    template<class T, class D = void>
    using IntrusiveSlugListSize = stdex::intrusive_slug_list_size_ptr<T, D, IntrusivePtr, IntrusivePtrBase, IntrusiveSlugHead>;
}
