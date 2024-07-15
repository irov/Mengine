#pragma once

#include "Kernel/Assertion.h"

#if defined(MENGINE_ASSERTION_DEBUG_ENABLE)
#   include "Kernel/Optional.h"

#   define MENGINE_ASSERTION_OPTIONAL(C) MENGINE_ASSERTION_FATAL((C).has_value() == true, "Optional '%s' empty value", #C)
#else
#   define MENGINE_ASSERTION_OPTIONAL(C)
#endif