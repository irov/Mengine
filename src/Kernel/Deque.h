#pragma once

#include "Kernel/StlAllocator.h"

#include <deque>

namespace Mengine
{
    template<class T, class A = StlAllocator<T>>
    using Deque = std::deque<T, A>;
}
