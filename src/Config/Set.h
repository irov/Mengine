#pragma once

#include "stdex/stl_allocator.h"

#include <set>

namespace Mengine
{
    template<class K, class L = std::less<K>, class A = stdex::stl_allocator<K>>
    using Set = std::set<K, L, A>;
}
