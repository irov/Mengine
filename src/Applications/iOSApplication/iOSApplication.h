#pragma once

#include "Interface/LoggerInterface.h"

#include "Config/Char.h"

namespace Mengine
{
    class iOSApplication
    {
    public:
        iOSApplication();
        ~iOSApplication();

    public:
        bool bootstrap( int32_t _argc, Char ** const _argv );

    public:
        bool initialize();
        void finalize();
        
    public:
        bool run();
        void stop();

    public:
        void loop();

    protected:
        bool initializeOptionsService_( int32_t argc, Char ** const _argv );
        bool initializeLoggerService_();

    protected:
        void finalizeLoggerService_();

    protected:
        LoggerInterfacePtr m_loggerMessageBox;
        LoggerInterfacePtr m_loggerStdio;
    };
}
