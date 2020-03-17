#pragma once

#include "Kernel/StlAllocator.h"

#include <set>

namespace Mengine
{
    template<class K, class L = std::less<K>, class A = StlAllocator<K>>
    using Set = std::set<K, L, A>;
}
