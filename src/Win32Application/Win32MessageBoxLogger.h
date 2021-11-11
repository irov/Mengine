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
        void log( ELoggerLevel _level, uint32_t _filter, uint32_t _color, const Char * _data, size_t _count ) override;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<Win32MessageBoxLogger, LoggerInterface> Win32MessageBoxLoggerPtr;
    //////////////////////////////////////////////////////////////////////////
}
