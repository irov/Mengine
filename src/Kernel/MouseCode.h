#pragma once

#include "Config/Typedef.h"

#ifndef MENGINE_INPUT_MAX_MOUSE_CODE
#define MENGINE_INPUT_MAX_MOUSE_CODE (3)
#endif

namespace Mengine
{
    enum EMouseCode : uint8_t
    {
        MC_LBUTTON = 0x00,
        MC_RBUTTON = 0x01,
        MC_MBUTTON = 0x02,
    };
}