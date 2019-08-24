#pragma once

#include "Config/Typedef.h"

namespace Mengine
{
    struct PlatformDateTime
    {
        uint32_t year;
        uint32_t month;
        uint32_t day;
        uint32_t hour;
        uint32_t minute;
        uint32_t second;
        uint32_t milliseconds;
    };
}