#pragma once

#include "Config/Config.h"

#ifdef MENGINE_ASSERTION_DEBUG_ENABLE
#   include "Kernel/Assertion.h"

#   include "Interface/AllocatorServiceInterface.h"

#   define MENGINE_ASSERTION_ALLOCATOR(C) ([=](){size_t alloc_count = ALLOCATOR_SERVICE()->find_alloc_count( C ); MENGINE_ASSERTION( alloc_count == 0, "'%s' memleak [%zu]", C , alloc_count);}())
#else
#   define MENGINE_ASSERTION_ALLOCATOR(C)
#endif