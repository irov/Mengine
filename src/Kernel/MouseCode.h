#pragma once

#include "Config/Typedef.h"

#define MENGINE_INPUT_MAX_MOUSE_CODE (3)

namespace Mengine
{
    enum EMouseCode : uint8_t
    {
        MC_LBUTTON = 0x00,
        MC_RBUTTON = 0x01,
        MC_MBUTTON = 0x02,
    };
}