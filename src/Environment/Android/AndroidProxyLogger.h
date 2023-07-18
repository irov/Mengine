#pragma once

#include "Environment/Android/AndroidIncluder.h"

#include "Kernel/LoggerBase.h"
#include "Kernel/String.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class AndroidProxyLogger
        : public LoggerBase
    {
    public:
        AndroidProxyLogger();
        ~AndroidProxyLogger() override;

    protected:
        bool _initializeLogger() override;
        void _finalizeLogger() override;

    protected:
        void _log( const LoggerMessage & _message ) override;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<AndroidProxyLogger, LoggerInterface> AndroidProxyLoggerPtr;
    //////////////////////////////////////////////////////////////////////////
}
