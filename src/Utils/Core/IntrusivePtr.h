#pragma once

#include "stdex/intrusive_ptr.h"
#include "stdex/intrusive_derived_ptr.h"

namespace Mengine
{
    template<class T>
    using IntrusivePtr = stdex::intrusive_ptr<T>;

    template<class T, class D>
    using IntrusiveDerivedPtr = stdex::intrusive_derived_ptr<T, D>;
}