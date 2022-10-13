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
        void log( const LoggerMessage & _message ) override;
        void flush() override;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<SDLStdioLogger, LoggerInterface> SDLStdioLoggerPtr;
    //////////////////////////////////////////////////////////////////////////
}
