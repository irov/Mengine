#pragma once

#include "Kernel/LoggerBase.h"

#include "Config/Lambda.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    typedef Lambda<void(const LoggerMessage &)> LambdaLoggerMessage;
    //////////////////////////////////////////////////////////////////////////
    class ProxyLogger
        : public LoggerBase
    {
    public:
        ProxyLogger();
        ~ProxyLogger() override;

    public:        
        void setLambda( const LambdaLoggerMessage & _lambda );
        const LambdaLoggerMessage & getLambda() const;

    public:
        void _log( const LoggerMessage & _message ) override;

    protected:
        LambdaLoggerMessage m_lambda;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<ProxyLogger, LoggerInterface> ProxyLoggerPtr;
    //////////////////////////////////////////////////////////////////////////
}
