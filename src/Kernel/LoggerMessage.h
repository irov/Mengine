#pragma once

#include "Kernel/LoggerLevel.h"
#include "Kernel/ConstString.h"
#include "Kernel/DateTime.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    struct LoggerMessage
    {
        ConstString category;
        PlatformDateTime dateTime;
        ELoggerLevel level;
        uint32_t filter;
        uint32_t color;
        const Char * data;
        size_t size;
    };
}