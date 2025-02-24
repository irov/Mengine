#pragma once

#include "Kernel/ServantBase.h"
#include "Kernel/LoggerBase.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class iOSLogger
        : public LoggerBase
    {
    public:
        iOSLogger();
        ~iOSLogger() override;

    public:
        void _log( const LoggerRecordInterfacePtr & _record ) override;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<iOSLogger, LoggerInterface> iOSLoggerPtr;
    //////////////////////////////////////////////////////////////////////////
}
