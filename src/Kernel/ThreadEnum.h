#pragma once

#include "Config/Config.h"

namespace Mengine
{
    enum EThreadPriority
    {
        ETP_LOWEST = -2,
        ETP_BELOW_NORMAL = -1,
        ETP_NORMAL = 0,
        ETP_ABOVE_NORMAL = 1,
        ETP_HIGHEST = 2,
        ETP_TIME_CRITICAL = 3
    };
}
