#pragma once

#include "Config/Typedef.h"

#define MENGINE_INPUT_MAX_MOUSE_BUTTON_CODE (4)

namespace Mengine
{
    enum EMouseButtonCode : uint8_t
    {
        MC_LBUTTON = 0x00,
        MC_RBUTTON = 0x01,
        MC_MBUTTON = 0x02,
        MC_XBUTTON = 0x03,
    };
}