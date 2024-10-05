#pragma once

#include "Kernel/Assertion.h"

#if defined(MENGINE_ASSERTION_DEBUG_ENABLE)
#   include "Interface/AllocatorSystemInterface.h"
#   include "Interface/UnknownAllocatorDebugReportInterface.h"

#   define MENGINE_ASSERTION_ALLOCATOR(C) ([=](){\
        Mengine::UnknownAllocatorDebugReportInterface * debugReport = ALLOCATOR_SYSTEM()->getUnknown();\
        if( debugReport == nullptr ){ return; }\
        size_t alloc_count = debugReport->findAllocatorReportInfo( C );\
        MENGINE_ASSERTION( alloc_count == 0, "'%s' memleak [%zu]", C , alloc_count);\
        debugReport->blockAllocator( C );\
    }())
#else
#   define MENGINE_ASSERTION_ALLOCATOR(C)
#endif