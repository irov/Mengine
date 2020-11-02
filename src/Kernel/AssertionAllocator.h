#pragma once

//////////////////////////////////////////////////////////////////////////
#if MENGINE_ASSERTION_DEBUG
#   include "Interface/AllocatorServiceInterface.h"
#   include "Kernel/Assertion.h"

#   define MENGINE_ASSERTION_ALLOCATOR(C) [](){uint32_t alloc_count = ALLOCATOR_SERVICE()->find_alloc_count( C ); MENGINE_ASSERTION( alloc_count == 0, "'%s' memleak [%d]", C , alloc_count);}()
#else
#   define MENGINE_ASSERTION_ALLOCATOR(C)
#endif
//////////////////////////////////////////////////////////////////////////