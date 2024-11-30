#pragma once

#include "Interface/LoggerInterface.h"

#include "Config/Char.h"

namespace Mengine
{
    class AndroidApplication
    {
    public:
        AndroidApplication();
        ~AndroidApplication();

    public:
        bool bootstrap( const Mengine::Char * _nativeLibraryDir, int32_t _argc, Char ** const _argv );

    public:
        bool initialize();
        void finalize();

    public:
        void loop();

    protected:
        bool initializeOptionsService_( int32_t argc, Char ** const _argv );
        bool initializeLoggerService_();

    protected:
        void finalizeLoggerService_();

    protected:
        LoggerInterfacePtr m_loggerAndroid;
    };
}
