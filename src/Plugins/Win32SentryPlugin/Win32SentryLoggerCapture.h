#pragma once

#include "Kernel/LoggerBase.h"

#include "Config/Char.h"

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
        void _log( const LoggerMessage & _message ) override;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<Win32SentryLoggerCapture, LoggerInterface> Win32SentryLoggerCapturePtr;
    //////////////////////////////////////////////////////////////////////////
}