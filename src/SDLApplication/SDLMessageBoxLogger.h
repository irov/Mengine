#pragma once

#include "Kernel/LoggerBase.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class SDLMessageBoxLogger
        : public LoggerBase
    {
    public:
        SDLMessageBoxLogger();
        ~SDLMessageBoxLogger() override;

    public:
        void _log( const LoggerRecordInterfacePtr & _record ) override;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<SDLMessageBoxLogger, LoggerInterface> SDLMessageBoxLoggerPtr;
    //////////////////////////////////////////////////////////////////////////
}
