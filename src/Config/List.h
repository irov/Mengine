#pragma once

#include "stdex/stl_allocator.h"

#include <list>

namespace Mengine
{
    template<class T, class A = stdex::stl_allocator<T> >
    using List = std::list<T, A>;
}
