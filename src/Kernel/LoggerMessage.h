#pragma once

#include "Kernel/LoggerLevel.h"
#include "Kernel/ConstString.h"
#include "Kernel/DateTime.h"

#include "Config/Timestamp.h"
#include "Config/Char.h"

#ifndef MENGINE_LOGGER_MAX_MESSAGE
#define MENGINE_LOGGER_MAX_MESSAGE 8192
#endif

#ifndef MENGINE_LOGGER_MAX_CATEGORY
#define MENGINE_LOGGER_MAX_CATEGORY 64
#endif

namespace Mengine
{    
    struct LoggerMessage
    {
        Timestamp timestamp;
        const Char * category;
        ConstString thread;
        ELoggerLevel level;
        uint32_t flag;
        uint32_t filter;
        uint32_t color;
        const Char * file;
        int32_t line;
        const Char * function;
        const Char * data;
        size_t size;
    };
}