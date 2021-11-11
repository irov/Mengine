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
        void log( ELoggerLevel _level, uint32_t _filter, uint32_t _color, const Char * _data, size_t _count ) override;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<Win32OutputDebugLogger, LoggerInterface> Win32OutputDebugLoggerPtr;
    //////////////////////////////////////////////////////////////////////////
}
