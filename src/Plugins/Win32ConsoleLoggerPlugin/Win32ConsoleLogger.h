#pragma once

#include "Kernel/LoggerBase.h"

#include "Config/Char.h"

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
        bool _initializeLogger() override;
        void _finalizeLogger() override;

    public:
        void log( ELoggerLevel _level, uint32_t _filter, uint32_t _color, const Char * _data, size_t _size ) override;
        void flush() override;

    protected:
        bool createConsole_();
        void removeConsole_();

    protected:
        bool m_createConsole;

        FILE * m_CONOUT;
        FILE * m_CONERR;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<Win32ConsoleLogger, LoggerInterface> Win32ConsoleLoggerPtr;
    //////////////////////////////////////////////////////////////////////////
}
