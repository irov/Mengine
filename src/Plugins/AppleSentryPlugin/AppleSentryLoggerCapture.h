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
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<AppleSentryLoggerCapture, LoggerInterface> AppleSentryLoggerCapturePtr;
    //////////////////////////////////////////////////////////////////////////
}
