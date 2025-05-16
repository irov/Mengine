#pragma once

#include "Kernel/StlAllocator.h"

#include <unordered_set>

namespace Mengine
{
    template<class T, class H = std::hash<T>, class Eq = std::equal_to<T>, class A = StlAllocator<T>>
    using UnorderedSet = std::unordered_set<T, H, Eq, A>;
}