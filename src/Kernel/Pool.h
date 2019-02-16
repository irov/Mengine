#pragma once

#include "stdex/template_pool.h"

namespace Mengine
{
    template<class T, uint32_t Count, class Allocator = stdex::stdex_pool_allocator_default>
    using Pool = stdex::template_pool<T, Count, Allocator>;
}