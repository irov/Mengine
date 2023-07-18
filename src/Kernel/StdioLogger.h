#pragma once

#include "Kernel/ServantBase.h"
#include "Kernel/LoggerBase.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class StdioLogger
        : public LoggerBase
    {
    public:
        StdioLogger();
        ~StdioLogger() override;

    public:
        void _log( const LoggerMessage & _message ) override;
        void _flush() override;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<StdioLogger, LoggerInterface> SDLStdioLoggerPtr;
    //////////////////////////////////////////////////////////////////////////
}
