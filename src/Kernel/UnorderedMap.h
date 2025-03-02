#pragma once

#include "Kernel/StlAllocator.h"

#include "Config/StdUtility.h"

#include <unordered_map>

namespace Mengine
{
    template<class K, class T, class H = std::hash<K>, class Eq = std::equal_to<K>, class A = StlAllocator<StdUtility::pair<const K, T>>>
    using UnorderedMap = std::unordered_map<K, T, H, Eq, A>;
}
