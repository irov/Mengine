#pragma once

#include "Kernel/StlAllocator.h"

#include "Config/StdUtility.h"

#include <map>

namespace Mengine
{
    template<class K, class T, class L = std::less<K>, class A = StlAllocator<StdUtility::pair<const K, T>>>
    using Map = std::map<K, T, L, A>;
}
