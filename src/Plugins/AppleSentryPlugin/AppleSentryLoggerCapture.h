#pragma once

#include "Kernel/LoggerBase.h"
#include "Kernel/String.h"

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
        bool _initializeLogger() override;
        void _finalizeLogger() override;

    protected:
        void log( const LoggerMessage & _message ) override;

    protected:
        String m_message;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<AppleSentryLoggerCapture, LoggerInterface> AppleSentryLoggerCapturePtr;
    //////////////////////////////////////////////////////////////////////////
}
