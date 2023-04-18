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
        ELoggerFlag flag;
        uint32_t filter;
        uint32_t color;
        const Char * file;
        int32_t line;
        const Char * data;
        size_t size;
    };
}