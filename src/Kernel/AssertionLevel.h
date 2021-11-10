#pragma once

#include "Config/Config.h"

namespace Mengine
{
    enum EAssertionLevel
    {
        ASSERTION_LEVEL_WARNING = 1,
        ASSERTION_LEVEL_ERROR = 2,
        ASSERTION_LEVEL_FATAL = 3,
        ASSERTION_LEVEL_EXCEPTION = 4,
        ASSERTION_LEVEL_CRITICAL = 5
    };
}