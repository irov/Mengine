#pragma once

#include "stdex/stl_allocator.h"

#include <unordered_map>

namespace Mengine
{
    template<class K, class T, class H = std::hash<K>, class Eq = std::equal_to<K>, class A = stdex::stl_allocator<std::pair<const K, T>>>
    using UnorderedMap = std::unordered_map<K, T, H, Eq, A>;
}
