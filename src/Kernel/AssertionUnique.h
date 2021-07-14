#pragma once

#include "Config/Config.h"

#if MENGINE_ASSERTION_DEBUG
#   include "Kernel/Assertion.h"
#   include "Config/UniqueId.h"

#   define MENGINE_ASSERTION_UNIQUE_INVALID(C) MENGINE_ASSERTION_FATAL((C) != Mengine::INVALID_UNIQUE_ID, "UniqueId '%s' is invalid", #C)
#else
#   define MENGINE_ASSERTION_UNIQUE_INVALID(C)
#endif