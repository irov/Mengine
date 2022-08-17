#pragma once

#include "Config/Config.h"

#ifdef MENGINE_ASSERTION_DEBUG_ENABLE
#   include "Kernel/Assertion.h"

#   include "Interface/AllocatorSystemInterface.h"
#   include "Interface/UnknownAllocatorDebugReportInterface.h"

#   define MENGINE_ASSERTION_ALLOCATOR(C) ([=](){\
        UnknownAllocatorDebugReportInterface * debugReport = ALLOCATOR_SYSTEM()->getUnknown();\
        if( debugReport == nullptr ){ return; }\
        size_t alloc_count = debugReport->findAllocatorReportInfo( C );\
        MENGINE_ASSERTION( alloc_count == 0, "'%s' memleak [%zu]", C , alloc_count);\
    }())
#else
#   define MENGINE_ASSERTION_ALLOCATOR(C)
#endif