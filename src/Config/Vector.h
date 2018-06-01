#pragma once

#include "stdex/stl_allocator.h"

#include <vector>

namespace Mengine
{
    template<class T, class A = stdex::stl_allocator<T>>
    using Vector = std::vector<T, A>;
}
