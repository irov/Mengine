#pragma once

#include "Config/Config.h"

namespace Mengine
{
    enum EUpdateMode
    {
        EUM_UNKNOWN,
        EUM_NODE_BASE,
        EUM_NODE_AFFECTOR,
        EUM_SERVICE_BEFORE,
        EUM_SERVICE_AFTER,
    };
}