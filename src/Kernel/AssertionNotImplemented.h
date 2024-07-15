#pragma once

#include "Kernel/Assertion.h"

#if defined(MENGINE_ASSERTION_DEBUG_ENABLE)
#   define MENGINE_ASSERTION_NOT_IMPLEMENTED() Mengine::Helper::Assertion( MENGINE_CODE_LIBRARY, ASSERTION_LEVEL_FATAL, "true", MENGINE_CODE_FILE, MENGINE_CODE_LINE, "This method not implemented, don't use that" )
#else
#   define MENGINE_ASSERTION_NOT_IMPLEMENTED()
#endif