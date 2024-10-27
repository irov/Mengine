#pragma once

#include "Interface/AnalyticsEventProviderInterface.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class AndroidAnalyticsEventProvider
        : public AnalyticsEventProviderInterface
        , public Factorable
    {
    public:
        AndroidAnalyticsEventProvider();
        ~AndroidAnalyticsEventProvider() override;

    protected:
        void onAnalyticsEvent( const AnalyticsEventInterfacePtr & _event ) override;
        void onAnalyticsScreenView( const ConstString & _screenType, const ConstString & _screenName ) override;

    protected:
        void onAnalyticsFlush() override;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<AndroidAnalyticsEventProvider> AndroidAnalyticsEventProviderPtr;
    //////////////////////////////////////////////////////////////////////////
}