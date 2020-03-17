#pragma once

#include "Kernel/StlAllocator.h"

#include <list>

namespace Mengine
{
    template<class T, class A = StlAllocator<T> >
    using List = std::list<T, A>;
}
