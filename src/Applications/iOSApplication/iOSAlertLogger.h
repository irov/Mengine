#pragma once

#include "Kernel/LoggerBase.h"

namespace Mengine
{
    class iOSAlertLogger
        : public LoggerBase
    {
    public:
        iOSAlertLogger();
        ~iOSAlertLogger() override;

    public:
        void _log( const LoggerRecordInterfacePtr & _record ) override;
    };

    typedef IntrusivePtr<iOSAlertLogger, LoggerInterface> iOSAlertLoggerPtr;
}
