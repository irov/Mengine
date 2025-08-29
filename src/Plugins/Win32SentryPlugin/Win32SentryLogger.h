#pragma once

#include "Kernel/LoggerBase.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class Win32SentryLogger
        : public LoggerBase
    {
    public:
        Win32SentryLogger();
        ~Win32SentryLogger() override;

    protected:
        bool _initializeLogger() override;
        void _finalizeLogger() override;

    protected:
        void _log( const LoggerRecordInterfacePtr & _record ) override;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<Win32SentryLogger, LoggerInterface> Win32SentryLoggerPtr;
    //////////////////////////////////////////////////////////////////////////
}
