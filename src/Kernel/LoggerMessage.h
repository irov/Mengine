#pragma once

#include "Kernel/LoggerLevel.h"
#include "Kernel/ConstString.h"
#include "Kernel/DateTime.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    struct LoggerMessage
    {
        const Char * category;
        PlatformDateTime dateTime;
        ELoggerLevel level;
        uint32_t flag;
        uint32_t filter;
        uint32_t color;
        const Char * file;
        int32_t line;
        const Char * data;
        size_t size;
    };
}