#pragma once

#include "Interface/AnalyticsEventProviderInterface.h"
#include "Interface/LoggerInterface.h"

#include "Win32OpButtonInterface.h"
#include "Win32OpButtonLoggerListener.h"

#include "Kernel/Map.h"
#include "Kernel/ServiceBase.h"
#include "Kernel/Vector.h"

#include "OpButton/OpButtonLayout.h"

namespace Mengine
{
    class Win32OpButtonService
        : public ServiceBase<Win32OpButtonServiceInterface>
    {
        DECLARE_FACTORABLE( Win32OpButtonService );

    public:
        Win32OpButtonService();
        ~Win32OpButtonService() override;

    public:
        const ServiceRequiredList & requiredServices() const override;

    public:
        bool addCommandButton( const ConstString & _tab, const ConstString & _identifier, const ConstString & _title, const Lambda<void()> & _callback ) override;
        void removeCommandTab( const ConstString & _tab ) override;

    protected:
        bool _initializeService() override;
        void _finalizeService() override;

    protected:
        void notifyBootstrapperCreateApplication_();
        void notifyPlatformRun_();
        void notifyPlatformUpdate_();

    protected:
        typedef Vector<OpButton::OpButtonButton *> VectorCommandButtons;

        struct CommandTabDesc
        {
            OpButton::OpButtonLayout * layout = nullptr;
            VectorCommandButtons buttons;
        };

        typedef Map<ConstString, CommandTabDesc> MapCommandTabs;
        MapCommandTabs m_commandTabs;

        LoggerInterfacePtr m_opbuttonLogger;
        AnalyticsEventProviderInterfacePtr m_analyticsEventProvider;

        Win32OpButtonLoggerListener m_opbuttonLoggerListener;

        bool m_sdkInitialized;
    };
}
