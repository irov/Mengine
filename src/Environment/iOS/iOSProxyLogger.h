#pragma once

#include "Kernel/LoggerBase.h"
#include "Kernel/String.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class iOSProxyLogger
        : public LoggerBase
    {
    public:
        iOSProxyLogger();
        ~iOSProxyLogger() override;

    protected:
        bool _initializeLogger() override;
        void _finalizeLogger() override;

    protected:
        void _log( const LoggerRecordInterfacePtr & message ) override;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<iOSProxyLogger, LoggerInterface> iOSProxyLoggerPtr;
    //////////////////////////////////////////////////////////////////////////
}
