#pragma once

#include "Kernel/LoggerBase.h"

#include "Config/Char.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class AppleSentryLoggerCapture
        : public LoggerBase
    {
    public:
        AppleSentryLoggerCapture();
        ~AppleSentryLoggerCapture() override;

    protected:
        void _log( const LoggerMessage & _message ) override;

    protected:
        Char m_buffer[MENGINE_LOGGER_MAX_MESSAGE] = {'\0'};
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<AppleSentryLoggerCapture, LoggerInterface> AppleSentryLoggerCapturePtr;
    //////////////////////////////////////////////////////////////////////////
}
