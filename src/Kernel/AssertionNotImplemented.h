#pragma once

#include "Kernel/Assertion.h"

#ifdef MENGINE_DEBUG
#   define MENGINE_ASSERTION_NOT_IMPLEMENTED() Mengine::Helper::Assertion( ASSERTION_LEVEL_WARNING, "true", __FILE__, __LINE__, "This method not implemented, don't use that" )
#else
#   define MENGINE_ASSERTION_NOT_IMPLEMENTED()
#endif