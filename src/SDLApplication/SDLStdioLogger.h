#pragma once

#include "Kernel/ServantBase.h"
#include "Kernel/LoggerBase.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class SDLStdioLogger
        : public LoggerBase
    {
    public:
		SDLStdioLogger();
        ~SDLStdioLogger() override;

    public:
        void log( ELoggerLevel _level, uint32_t _flag, const Char * _data, uint32_t _count ) override;
        void flush() override;
    };
}
