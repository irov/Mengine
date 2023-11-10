#pragma once

#include "Config/Typedef.h"

#define MENGINE_INPUT_MAX_MOUSE_BUTTON_CODE (5)

namespace Mengine
{
    enum EMouseButtonCode : uint8_t
    {
        MC_LBUTTON = 0x00,
        MC_MBUTTON = 0x01,
        MC_RBUTTON = 0x02,
        MC_X1BUTTON = 0x03,
        MC_X2BUTTON = 0x04,
    };
}