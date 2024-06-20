#pragma once

#include "Config/Config.h"

namespace Mengine
{
    enum EHttpRequestFlags
    {
        EHRF_NONE = 0,
        EHRF_RECEIVE_HEADERS = 1 << 0,
        EHRE_LOW_PRIORITY = 1 << 1,
    };
}