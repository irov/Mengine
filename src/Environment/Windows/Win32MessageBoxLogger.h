#pragma once

#include "Kernel/LoggerBase.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class Win32MessageBoxLogger
        : public LoggerBase
    {
    public:
        Win32MessageBoxLogger();
        ~Win32MessageBoxLogger() override;

    public:
        void _log( const LoggerMessage & _message ) override;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<Win32MessageBoxLogger, LoggerInterface> Win32MessageBoxLoggerPtr;
    //////////////////////////////////////////////////////////////////////////
}
