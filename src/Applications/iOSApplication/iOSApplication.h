#pragma once

#include "Interface/LoggerInterface.h"

#include "Config/Char.h"

#import <Foundation/Foundation.h>

namespace Mengine
{
    class iOSApplication
    {
    public:
        iOSApplication();
        ~iOSApplication();

    public:
        bool bootstrap( NSArray<NSString *> * _arguments );
        
    public:
        bool run();
        void stop();
        
    public:
        void finalize();

    protected:
        bool initializeOptionsService_( NSArray<NSString *> * _arguments );
        bool initializeLoggerService_();

    protected:
        void finalizeLoggerService_();

    protected:
        LoggerInterfacePtr m_loggerMessageBox;
        LoggerInterfacePtr m_loggerStdio;
    };
}
