#pragma once

#include "Kernel/StlAllocator.h"

#include "stdex/dynamic_array.h"

namespace Mengine
{
    template<class T, class A = StlAllocator<T>>
    using DynamicArray = stdex::dynamic_array<T, A>;
}