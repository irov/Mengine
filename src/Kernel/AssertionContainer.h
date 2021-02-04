#pragma once

#include "Config/Config.h"

#if MENGINE_ASSERTION_DEBUG
#   include "Kernel/Assertion.h"

#   define MENGINE_ASSERTION_CONTAINER_EMPTY(C) MENGINE_ASSERTION_FATAL((C).empty() == true, "Container '%s' not empty", #C)
#else
#   define MENGINE_ASSERTION_CONTAINER_EMPTY(C)
#endif