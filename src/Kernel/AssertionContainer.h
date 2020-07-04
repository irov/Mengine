#pragma once

#include "Kernel/Assertion.h"

//////////////////////////////////////////////////////////////////////////
#ifdef MENGINE_ASSERTION_DEBUG
#   define MENGINE_ASSERTION_CONTAINER_EMPTY(C) MENGINE_ASSERTION_FATAL((C).empty() == true, "Container '%s' not empty", #C)
#else
#   define MENGINE_ASSERTION_CONTAINER_EMPTY(C)
#endif
//////////////////////////////////////////////////////////////////////////