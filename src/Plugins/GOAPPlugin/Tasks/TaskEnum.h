#pragma once

#include "Config/Config.h"

namespace Mengine
{
    enum ETaskFlags
    {
        ETASK_FLAG_NONE = 0,
        ETASK_FLAG_NOREWIND = 1 << 0,
    };
}