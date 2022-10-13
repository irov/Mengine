#pragma once

#include "Kernel/LoggerBase.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class Win32OutputDebugLogger
        : public LoggerBase
    {
    public:
        Win32OutputDebugLogger();
        ~Win32OutputDebugLogger() override;

    public:
        void log( const LoggerMessage & _message ) override;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<Win32OutputDebugLogger, LoggerInterface> Win32OutputDebugLoggerPtr;
    //////////////////////////////////////////////////////////////////////////
}
