#pragma once

#include "Interface/AnalyticsEventProviderInterface.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class Win32OpButtonAnalyticsEventProvider
        : public AnalyticsEventProviderInterface
        , public Factorable
    {
    public:
        Win32OpButtonAnalyticsEventProvider();
        ~Win32OpButtonAnalyticsEventProvider() override;

    public:
        void onAnalyticsEvent( const AnalyticsEventInterfacePtr & _event ) override;
        void onAnalyticsScreenView( const ConstString & _screenType, const ConstString & _screenName ) override;
        void onAnalyticsFlush() override;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<Win32OpButtonAnalyticsEventProvider, AnalyticsEventProviderInterface> Win32OpButtonAnalyticsEventProviderPtr;
    //////////////////////////////////////////////////////////////////////////
}
