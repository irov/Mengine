#pragma once

#include "Kernel/Assertion.h"

//////////////////////////////////////////////////////////////////////////
#ifdef MENGINE_ASSERTION_DEBUG
#   define MENGINE_ASSERTION_TODO() Mengine::Helper::Assertion( ASSERTION_LEVEL_WARNING, "true", MENGINE_CODE_FUNCTION, MENGINE_CODE_LINE, "This method not implemented, todo.." )
#else
#   define MENGINE_ASSERTION_TODO()
#endif
//////////////////////////////////////////////////////////////////////////