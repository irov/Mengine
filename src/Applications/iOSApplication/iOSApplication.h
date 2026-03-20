#pragma once

#include "Interface/LoggerInterface.h"
#include "Interface/ArgumentsInterface.h"

#include "Config/Char.h"

namespace Mengine
{
    class iOSApplication
    {
    public:
        iOSApplication();
        ~iOSApplication();

    public:
        bool bootstrap( const ArgumentsInterfacePtr & _arguments );
        
    public:
        bool run();
        void stop();
        
    public:
        void finalize();

    protected:
        bool initializeOptionsService_( const ArgumentsInterfacePtr & _arguments );
        bool initializeLoggerService_();

    protected:
        void finalizeLoggerService_();

    protected:
        LoggerInterfacePtr m_loggerMessageBox;
        LoggerInterfacePtr m_loggerStdio;
    };
}
