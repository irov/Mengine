#pragma once

#include "Kernel/Assertion.h"

#ifdef MENGINE_DEBUG
#   define MENGINE_ASSERTION_NOT_IMPLEMENTED() Mengine::Helper::Assertion( ASSERTION_LEVEL_WARNING, "true", MENGINE_CODE_FILE, MENGINE_CODE_LINE, "This method not implemented, don't use that" )
#else
#   define MENGINE_ASSERTION_NOT_IMPLEMENTED()
#endif