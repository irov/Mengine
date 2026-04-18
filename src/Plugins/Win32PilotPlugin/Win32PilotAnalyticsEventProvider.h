#pragma once

#include "Interface/AnalyticsEventProviderInterface.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class Win32PilotAnalyticsEventProvider
        : public AnalyticsEventProviderInterface
        , public Factorable
    {
    public:
        Win32PilotAnalyticsEventProvider();
        ~Win32PilotAnalyticsEventProvider() override;

    public:
        void onAnalyticsEvent( const AnalyticsEventInterfacePtr & _event ) override;
        void onAnalyticsScreenView( const ConstString & _screenType, const ConstString & _screenName ) override;
        void onAnalyticsFlush() override;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<Win32PilotAnalyticsEventProvider, AnalyticsEventProviderInterface> Win32PilotAnalyticsEventProviderPtr;
    //////////////////////////////////////////////////////////////////////////
}
