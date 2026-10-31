#pragma once

#include "Interface/LoggerInterface.h"

#include "Kernel/Configuration.h"

#include "Config/Char.h"

namespace Mengine
{
    class EmscriptenApplication
    {
    public:
        EmscriptenApplication();
        ~EmscriptenApplication();

    public:
        bool bootstrap( int32_t _argc, Char ** const _argv, const Configuration & _configuration );
        bool initialize();
        void loop();
        void finalize();
        void frame();

    protected:
        bool initializeOptionsService_( int32_t _argc, Char ** const _argv );
        bool initializeLoggerService_();
        void finalizeLoggerService_();

    protected:
        LoggerInterfacePtr m_loggerStdio;
        double m_previousTime;
        bool m_finalized;
    };
}
