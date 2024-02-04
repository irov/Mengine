#pragma once

#include "Kernel/LoggerLevel.h"
#include "Kernel/ConstString.h"
#include "Kernel/DateTime.h"
#include "Kernel/VectorChar.h"

#include "Config/Timestamp.h"

#ifndef MENGINE_LOGGER_MAX_MESSAGE
#define MENGINE_LOGGER_MAX_MESSAGE 16384
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
        ConstString threadName;
        ELoggerLevel level;
        uint32_t flag;
        uint32_t filter;
        uint32_t color;
        const Char * file;
        int32_t line;
        const Char * data;
        size_t size;
    };

    struct LoggerRecord
    {
        Timestamp timestamp;
        Char category[MENGINE_LOGGER_MAX_CATEGORY];
        ConstString threadName;
        ELoggerLevel level;
        uint32_t flag;
        uint32_t filter;
        uint32_t color;
        const Char * file;
        int32_t line;
        VectorChar data;
    };

    namespace Helper
    {
        void recordLoggerMessage( const LoggerMessage & _message, LoggerRecord * const _record );
        void unrecordLoggerMessage( const LoggerRecord & _record, LoggerMessage * const _message );
    }
}