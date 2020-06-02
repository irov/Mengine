#pragma once

#include "Kernel/StlAllocator.h"

#include <vector>

namespace Mengine
{
    template<class T>
    struct VectorStaticAssertionBoolSpecialization
    {
        static_assert(std::is_same<T, bool>::value == false, "vector assertion bool specialization");

        typedef T type;
    };

    template<class T, class A = StlAllocator<T>>
    using Vector = std::vector<typename VectorStaticAssertionBoolSpecialization<T>::type, A>;
}
