#pragma once

#include "Kernel/LoggerBase.h"

#include "Config/String.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class Win32ConsoleLogger
        : public LoggerBase
    {
    public:
        Win32ConsoleLogger();
        ~Win32ConsoleLogger() override;

    public:
        bool initialize() override;
        void finalize() override;

    public:
        void log( EMessageLevel _level, uint32_t _flag, const Char * _data, uint32_t _count ) override;
        void flush() override;

    protected:
        void createConsole_();
        void removeConsole_();

    protected:
        bool m_createConsole;

        FILE * m_CONOUT;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<Win32ConsoleLogger, LoggerInterface> Win32ConsoleLoggerPtr;
    //////////////////////////////////////////////////////////////////////////
}
