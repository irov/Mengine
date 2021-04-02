#pragma once

#include "Environment/Windows/WindowsIncluder.h"

#include "Interface/LoggerInterface.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class Win32Application
    {
    public:
        Win32Application();
        ~Win32Application();

    public:
        bool initialize();
        void finalize();

    public:
        void loop();

    protected:        
        bool initializeOptionsService_();
        bool initializeLoggerService_();
        bool initializeArchiveService_();

    protected:
        void finalizeLoggerService_();
        
    protected:
#ifdef MENGINE_PLUGIN_MENGINE_DLL
        HINSTANCE m_hInstance;
#endif

        LoggerInterfacePtr m_loggerMessageBox;
        LoggerInterfacePtr m_loggerOutputDebug;
    };
    //////////////////////////////////////////////////////////////////////////
}
