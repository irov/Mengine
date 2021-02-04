#pragma once

#include "Config/Config.h"

#if MENGINE_ASSERTION_DEBUG
#   include "Kernel/Assertion.h"

#   define MENGINE_ASSERTION_NOT_IMPLEMENTED() Mengine::Helper::Assertion( ASSERTION_LEVEL_FATAL, "true", MENGINE_CODE_FUNCTION, MENGINE_CODE_LINE, "This method not implemented, don't use that" )
#else
#   define MENGINE_ASSERTION_NOT_IMPLEMENTED()
#endif