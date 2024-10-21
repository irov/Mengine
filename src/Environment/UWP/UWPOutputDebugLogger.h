#pragma once

#include "Kernel/LoggerBase.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class UWPOutputDebugLogger
        : public LoggerBase
    {
    public:
        UWPOutputDebugLogger();
        ~UWPOutputDebugLogger() override;

    public:
        void _log( const LoggerRecordInterfacePtr & _record ) override;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<UWPOutputDebugLogger, LoggerInterface> Win32OutputDebugLoggerPtr;
    //////////////////////////////////////////////////////////////////////////
}
