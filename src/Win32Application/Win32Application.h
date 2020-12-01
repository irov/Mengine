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
        bool initializeFileService_();
        bool initializeOptionsService_();
        bool initializeLoggerService_();
        bool initializeArchiveService_();

    protected:
        void finalizeLoggerService_();
        void finalizeFileService_();

    protected:
        HINSTANCE m_hInstance;

        LoggerInterfacePtr m_loggerMessageBox;
    };
    //////////////////////////////////////////////////////////////////////////
}
