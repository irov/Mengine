#pragma once

#include "Kernel/LoggerBase.h"

#include "Environment/Windows/WindowsIncluder.h"

#include <setupapi.h>

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class Win32ExtraFileLogger
        : public LoggerBase
    {
    public:
        Win32ExtraFileLogger();
        ~Win32ExtraFileLogger() override;

    protected:
        bool _initializeLogger() override;
        void _finalizeLogger() override;

    protected:
        void _log( const LoggerRecordInterfacePtr & _record ) override;

    protected:
        void _flush() override;

    protected:
        void writeMessage_( const Char * _msg );

    protected:
        HANDLE m_hLogFile;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<Win32ExtraFileLogger, LoggerInterface> Win32ExtraFileLoggerPtr;
    //////////////////////////////////////////////////////////////////////////
}
