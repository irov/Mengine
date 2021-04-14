#pragma once

#include "Config/Config.h"
#include "Config/UniqueId.h"

#if MENGINE_ASSERTION_DEBUG
#   include "Kernel/Assertion.h"

#   define MENGINE_ASSERTION_UNIQUE_INVALID(C) MENGINE_ASSERTION_FATAL((C) != INVALIDATE_UNIQUE_ID, "UniqueId '%s' is invalid", #C)
#else
#   define MENGINE_ASSERTION_UNIQUE_INVALID(C)
#endif