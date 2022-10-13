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
        void log( const LoggerMessage & _message ) override;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<SDLOutputDebugLogger, LoggerInterface> SDLOutputDebugLoggerPtr;
    //////////////////////////////////////////////////////////////////////////
}
