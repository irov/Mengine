#pragma once

#include "Kernel/LoggerBase.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class Win32OpButtonLogger
        : public LoggerBase
    {
    public:
        Win32OpButtonLogger();
        ~Win32OpButtonLogger() override;

    protected:
        void _log( const LoggerRecordInterfacePtr & _record ) override;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<Win32OpButtonLogger, LoggerInterface> Win32OpButtonLoggerPtr;
    //////////////////////////////////////////////////////////////////////////
}
