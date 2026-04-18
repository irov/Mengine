#pragma once

#include "Kernel/LoggerBase.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class Win32PilotLogger
        : public LoggerBase
    {
    public:
        Win32PilotLogger();
        ~Win32PilotLogger() override;

    protected:
        void _log( const LoggerRecordInterfacePtr & _record ) override;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<Win32PilotLogger, LoggerInterface> Win32PilotLoggerPtr;
    //////////////////////////////////////////////////////////////////////////
}
