#pragma once

#include "Interface/LoggerInterface.h"

#include "Config/Char.h"

namespace Mengine
{
    class UnixApplication
    {
    public:
        UnixApplication();
        ~UnixApplication();

    public:
        bool bootstrap( int32_t _argc, Char ** const _argv );
        bool initialize();
        void loop();
        void finalize();

    protected:
        bool initializeOptionsService_( int32_t _argc, Char ** const _argv );
        bool initializeLoggerService_();
        void finalizeLoggerService_();

    protected:
        LoggerInterfacePtr m_loggerStdio;
    };
}
