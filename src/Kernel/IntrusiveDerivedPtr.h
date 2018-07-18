#pragma once

#include "stdex/intrusive_derived_ptr.h"

namespace Mengine
{
    template<class T, class D>
    using IntrusiveDerivedPtr = stdex::intrusive_derived_ptr<T, D>;
}