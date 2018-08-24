#pragma once

#include "stdex/stl_allocator.h"

#include <deque>

namespace Mengine
{
    template<class T, class A = stdex::stl_allocator<T>>
    using Deque = std::deque<T, A>;
}
