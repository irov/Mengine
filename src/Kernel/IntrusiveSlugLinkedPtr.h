#pragma once

#include "Kernel/IntrusivePtr.h"
#include "Kernel/IntrusivePtrBase.h"

#include "stdex/intrusive_slug_linked_ptr.h"

namespace Mengine
{
    template<class Tag, class Derived, template <class, class> class IntrusivePtr, class IntrusivePtrBase>
    using IntrusiveSlugLinkedPtr = stdex::intrusive_slug_linked_ptr<Tag, Derived, IntrusivePtr, IntrusivePtrBase>;
}
