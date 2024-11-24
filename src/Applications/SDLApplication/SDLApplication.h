#pragma once

#include "Interface/LoggerInterface.h"

#include "Config/Char.h"

namespace Mengine
{
    class SDLApplication
    {
    public:
        SDLApplication();
        ~SDLApplication();

    public:
        bool bootstrap( int32_t _argc, const Char ** const _argv );

    public:
        bool initialize();
        void finalize();

    public:
        void loop();

    protected:
        bool initializeOptionsService_( int32_t argc, const Char ** const _argv );
        bool initializeLoggerService_();

    protected:
        void finalizeLoggerService_();

    protected:
        LoggerInterfacePtr m_loggerMessageBox;
        LoggerInterfacePtr m_loggerStdio;

#if defined(MENGINE_WINDOWS_DEBUG)
        LoggerInterfacePtr m_loggerOutputDebug;
#endif
    };
}
