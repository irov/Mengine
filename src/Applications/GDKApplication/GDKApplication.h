#pragma once

#include "Configuration/Configurations.h"

#if defined(MENGINE_PLUGIN_MENGINE_SHARED)
#   include "Environment/Windows/WindowsIncluder.h"
#endif

#include "Interface/LoggerInterface.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class GDKApplication
    {
    public:
        GDKApplication();
        ~GDKApplication();

    public:
        bool initialize();
        void finalize();

    public:
        void loop();

    protected:
        bool initializeOptionsService_();
        bool initializeLoggerService_();

    protected:
        void finalizeLoggerService_();
        
    protected:
#if defined(MENGINE_PLUGIN_MENGINE_SHARED)
        HINSTANCE m_hInstance;
#endif

        LoggerInterfacePtr m_loggerConsole;
        LoggerInterfacePtr m_loggerMessageBox;

#if defined(MENGINE_WINDOWS_DEBUG)
        LoggerInterfacePtr m_loggerOutputDebug;
#endif

        LoggerInterfacePtr m_loggerExtraFile;
    };
    //////////////////////////////////////////////////////////////////////////
}