#pragma once

#include "Kernel/LoggerBase.h"

namespace Mengine
{
    class AndroidLogger
        : public LoggerBase
    {
    public:
        AndroidLogger();
        ~AndroidLogger() override;

    public:
        void log( ELoggerLevel _level, uint32_t _flag, uint32_t _color, const Char * _data, size_t _count ) override;
        void flush() override;
    };
}
