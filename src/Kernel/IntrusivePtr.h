#pragma once

#include "stdex/intrusive_ptr.h"

namespace Mengine
{
    template<class T, class D = void>
    using IntrusivePtr = stdex::intrusive_ptr<T, D>;
}
