#pragma once

#include "Win32PilotInterface.h"

#include "Win32PilotLoggerListener.h"

#include "Interface/LoggerInterface.h"
#include "Interface/AnalyticsEventProviderInterface.h"

#include "Kernel/ServiceBase.h"

namespace Mengine
{
    class Win32PilotService
        : public ServiceBase<Win32PilotServiceInterface>
    {
        DECLARE_FACTORABLE( Win32PilotService );

    public:
        Win32PilotService();
        ~Win32PilotService() override;

    public:
        bool _initializeService() override;
        void _finalizeService() override;

    protected:
        void notifyBootstrapperCreateApplication_();
        void notifyPlatformUpdate_();

    protected:
        LoggerInterfacePtr m_pilotLogger;
        AnalyticsEventProviderInterfacePtr m_analyticsEventProvider;

        Win32PilotLoggerListener m_pilotLoggerListener;

        bool m_sdkInitialized;
    };
}
