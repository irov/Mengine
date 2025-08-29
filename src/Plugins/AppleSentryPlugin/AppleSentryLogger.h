#pragma once

#include "Kernel/LoggerBase.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class AppleSentryLogger
        : public LoggerBase
    {
    public:
        AppleSentryLogger();
        ~AppleSentryLogger() override;

    protected:
        bool _initializeLogger() override;
        void _finalizeLogger() override;

    protected:
        void _log( const LoggerRecordInterfacePtr & _record ) override;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<AppleSentryLogger, LoggerInterface> AppleSentryLoggerPtr;
    //////////////////////////////////////////////////////////////////////////
}
