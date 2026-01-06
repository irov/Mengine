#pragma once

#include "Kernel/LoggerBase.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class SDL3MessageBoxLogger
        : public LoggerBase
    {
    public:
        SDL3MessageBoxLogger();
        ~SDL3MessageBoxLogger() override;

    public:
        void _log( const LoggerRecordInterfacePtr & _record ) override;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<SDL3MessageBoxLogger, LoggerInterface> SDL3MessageBoxLoggerPtr;
    //////////////////////////////////////////////////////////////////////////
}



