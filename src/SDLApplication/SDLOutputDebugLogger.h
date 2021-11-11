#pragma once

#include "Kernel/LoggerBase.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class SDLOutputDebugLogger
        : public LoggerBase
    {
    public:
        SDLOutputDebugLogger();
        ~SDLOutputDebugLogger() override;

    public:
        void log( ELoggerLevel _level, uint32_t _filter, uint32_t _color, const Char * _data, size_t _count ) override;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<SDLOutputDebugLogger, LoggerInterface> SDLOutputDebugLoggerPtr;
    //////////////////////////////////////////////////////////////////////////
}
