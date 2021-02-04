#pragma once

#include "Config/Config.h"

#if MENGINE_ASSERTION_DEBUG
#   include "Kernel/Assertion.h"
#   include "Kernel/Optional.h"

#   define MENGINE_ASSERTION_OPTIONAL(C) MENGINE_ASSERTION_FATAL((C).has_value() == true, "Optional '%s' empty value", #C)
#else
#   define MENGINE_ASSERTION_OPTIONAL(C)
#endif