#pragma once

#include "Kernel/ServantBase.h"
#include "Kernel/LoggerBase.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class AppleNSLogger
        : public LoggerBase
    {
    public:
        AppleNSLogger();
        ~AppleNSLogger() override;

    public:
        void _log( const LoggerRecordInterfacePtr & _record ) override;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<AppleNSLogger, LoggerInterface> AppleNSLoggerPtr;
    //////////////////////////////////////////////////////////////////////////
}
