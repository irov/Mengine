#pragma once

#include "Config/Typedef.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    struct UpdateContext
    {
        uint32_t revision;
        float current;
        float time;
    };
}