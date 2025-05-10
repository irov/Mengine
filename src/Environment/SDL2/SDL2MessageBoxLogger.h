#pragma once

#include "Kernel/LoggerBase.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class SDL2MessageBoxLogger
        : public LoggerBase
    {
    public:
        SDL2MessageBoxLogger();
        ~SDL2MessageBoxLogger() override;

    public:
        void _log( const LoggerRecordInterfacePtr & _record ) override;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<SDL2MessageBoxLogger, LoggerInterface> SDL2MessageBoxLoggerPtr;
    //////////////////////////////////////////////////////////////////////////
}
