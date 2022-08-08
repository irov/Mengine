#pragma once

#include "Config/Config.h"

#ifdef MENGINE_ASSERTION_DEBUG_ENABLE
#   include "Kernel/Assertion.h"
#   include "Kernel/Optional.h"

#   define MENGINE_ASSERTION_OPTIONAL(C) MENGINE_ASSERTION_FATAL((C).has_value() == true, "Optional '%s' empty value", #C)
#else
#   define MENGINE_ASSERTION_OPTIONAL(C)
#endif