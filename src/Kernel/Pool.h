#pragma once

#include "Kernel/PoolAllocator.h"

#include "stdex/template_pool.h"

namespace Mengine
{
    template<class T, uint32_t Count, class Allocator = PoolAllocator>
    using Pool = stdex::template_pool<T, Count, Allocator>;
}