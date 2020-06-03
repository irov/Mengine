#pragma once

#include "Interface/NotificationServiceInterface.h"
#include "Interface/LoggerInterface.h"
#include "Interface/UnicodeSystemInterface.h"
#include "Interface/ConfigServiceInterface.h"

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

    protected:
        ServiceProviderInterface * m_serviceProvider;

        LoggerInterfacePtr m_loggerMessageBox;
    };
}
