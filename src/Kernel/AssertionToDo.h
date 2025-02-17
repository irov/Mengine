#pragma once

#include "Kernel/Assertion.h"

#if defined(MENGINE_ASSERTION_DEBUG_ENABLE)
#   define MENGINE_ASSERTION_TODO() Mengine::Helper::Assertion( MENGINE_CODE_LIBRARY, ASSERTION_LEVEL_WARNING, "true", MENGINE_CODE_FILE, MENGINE_CODE_LINE, MENGINE_CODE_FUNCTION, "This method not implemented, todo.." )
#else
#   define MENGINE_ASSERTION_TODO()
#endif