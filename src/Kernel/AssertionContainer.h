#pragma once

#include "Config/Config.h"

#if defined(MENGINE_ASSERTION_DEBUG_ENABLE)
#   include "Kernel/Assertion.h"

#   define MENGINE_ASSERTION_CONTAINER_EMPTY(C) MENGINE_ASSERTION_FATAL((C).empty() == true, "Container '%s' not empty", #C)
#else
#   define MENGINE_ASSERTION_CONTAINER_EMPTY(C)
#endif