#pragma once

#include "Kernel/LoggerBase.h"
#include "Kernel/String.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class Win32SentryLoggerCapture
        : public LoggerBase
    {
    public:
        Win32SentryLoggerCapture();
        ~Win32SentryLoggerCapture() override;

    protected:
        bool _initializeLogger() override;
        void _finalizeLogger() override;

    protected:
        void _log( const LoggerMessage & _message ) override;

    protected:
        String m_message;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<Win32SentryLoggerCapture, LoggerInterface> Win32SentryLoggerCapturePtr;
    //////////////////////////////////////////////////////////////////////////
}