#pragma once

#include "Kernel/LoggerBase.h"

#include "Config/Lambda.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    typedef Lambda<void( const LoggerRecordInterfacePtr & )> LambdaLoggerRecord;
    //////////////////////////////////////////////////////////////////////////
    class ProxyLogger
        : public LoggerBase
    {
    public:
        ProxyLogger();
        ~ProxyLogger() override;

    public:        
        void setLambda( const LambdaLoggerRecord & _lambda );
        const LambdaLoggerRecord & getLambda() const;

    public:
        void _log( const LoggerRecordInterfacePtr & _record ) override;

    protected:
        LambdaLoggerRecord m_lambda;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<ProxyLogger, LoggerInterface> ProxyLoggerPtr;
    //////////////////////////////////////////////////////////////////////////
}
