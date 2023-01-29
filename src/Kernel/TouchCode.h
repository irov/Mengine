#pragma once

#include "Config/Typedef.h"

#define MENGINE_INPUT_MAX_TOUCH 16

namespace Mengine
{
    enum ETouchCode : uint8_t
    {
        TC_TOUCH0 = 0x00,
        TC_TOUCH1 = 0x01,
        TC_TOUCH2 = 0x02,
        TC_TOUCH3 = 0x03,
        TC_TOUCH4 = 0x04,
        TC_TOUCH5 = 0x05,
        TC_TOUCH6 = 0x06,
        TC_TOUCH7 = 0x07,
        TC_TOUCH8 = 0x08,
        TC_TOUCH9 = 0x09,
        TC_TOUCH10 = 0x0A,
        TC_TOUCH11 = 0x0B,
        TC_TOUCH12 = 0x0C,
        TC_TOUCH13 = 0x0D,
        TC_TOUCH14 = 0x0E,
        TC_TOUCH15 = 0x0F,
        TC_TOUCH_INVALID = 0xFF,
    };
}