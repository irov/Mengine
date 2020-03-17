#pragma once

#include "Kernel/StlAllocator.h"

#include <map>

namespace Mengine
{
    template<class K, class T, class L = std::less<K>, class A = StlAllocator<std::pair<const K, T>>>
    using Map = std::map<K, T, L, A>;
}
