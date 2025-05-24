#pragma once

#include "Config/Config.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    enum ETextHorizontAlign : uint32_t
    {
        ETFHA_LEFT = 0,
        ETFHA_CENTER = 1,
        ETFHA_RIGHT = 2,
    };
    //////////////////////////////////////////////////////////////////////////
    enum ETextVerticalAlign : uint32_t
    {
        ETFVA_BOTTOM = 0,
        ETFVA_CENTER = 1,
        ETFVA_TOP = 2,
    };
    //////////////////////////////////////////////////////////////////////////
    enum ETextParams : uint32_t
    {
        EFP_NONE = 0,
        EFP_FONT = 1 << 0,
        EFP_COLOR_FONT = 1 << 1,
        EFP_LINE_OFFSET = 1 << 2,
        EFP_CHAR_OFFSET = 1 << 3,
        EFP_MAX_LENGTH = 1 << 4,
        EFP_MAX_HEIGHT = 1 << 5,
        EFP_HORIZONTAL_ALIGN = 1 << 6,
        EFP_VERTICAL_ALIGN = 1 << 7,
        EFP_CHAR_SCALE = 1 << 8,
        EFP_AUTO_SCALE = 1 << 9,
        EFP_JUSTIFY = 1 << 10,
    };
    //////////////////////////////////////////////////////////////////////////
}