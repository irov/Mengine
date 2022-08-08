#pragma once

#include "Config/Config.h"

#ifdef MENGINE_ASSERTION_DEBUG_ENABLE
#   include "Kernel/Assertion.h"

#   define MENGINE_ASSERTION_TODO() Mengine::Helper::Assertion( ASSERTION_LEVEL_WARNING, "true", MENGINE_CODE_FUNCTION, MENGINE_CODE_LINE, "This method not implemented, todo.." )
#else
#   define MENGINE_ASSERTION_TODO()
#endif