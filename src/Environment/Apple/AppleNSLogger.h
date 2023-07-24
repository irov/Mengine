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
        void _log( const LoggerMessage & _message ) override;

    protected:
        Char m_message[MENGINE_LOGGER_MAX_MESSAGE] = {'\0'};
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<AppleNSLogger, LoggerInterface> AppleNSLoggerPtr;
    //////////////////////////////////////////////////////////////////////////
}
