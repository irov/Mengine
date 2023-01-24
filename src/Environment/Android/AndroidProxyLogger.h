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
        void log( const LoggerMessage & _message ) override;

    protected:
        Char m_loggerMessage[MENGINE_LOGGER_MAX_MESSAGE];
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<AndroidProxyLogger, LoggerInterface> AndroidProxyLoggerPtr;
    //////////////////////////////////////////////////////////////////////////
}
