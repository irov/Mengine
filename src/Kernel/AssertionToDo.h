#pragma once

#include "Config/Config.h"

#if defined(MENGINE_ASSERTION_DEBUG_ENABLE)
#   include "Kernel/Assertion.h"

#   define MENGINE_ASSERTION_TODO() Mengine::Helper::Assertion( MENGINE_CODE_LIBRARY, ASSERTION_LEVEL_WARNING, "true", MENGINE_CODE_FILE, MENGINE_CODE_LINE, "This method not implemented, todo.." )
#else
#   define MENGINE_ASSERTION_TODO()
#endif