#pragma once

#include "Kernel/StlAllocator.h"

#include <vector>

namespace Mengine
{
    template<class T, class A = StlAllocator<T>>
    using Vector = std::vector<T, A>;
}
