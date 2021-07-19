#pragma once

#include "Config/Typedef.h"

#ifndef MENGINE_INPUT_MAX_WHEEL_CODE
#define MENGINE_INPUT_MAX_WHEEL_CODE (1)
#endif

namespace Mengine
{
    enum EWheelCode : uint8_t
    {
        WC_CENTRAL = 0x00,
    };
}