#pragma once

#include "Interface/AnalyticsEventProviderInterface.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class iOSAnalyticsEventProvider
        : public AnalyticsEventProviderInterface
        , public Factorable
    {
    public:
        iOSAnalyticsEventProvider();
        ~iOSAnalyticsEventProvider() override;

    protected:
        void onAnalyticsEvent( const AnalyticsEventInterfacePtr & _event ) override;
        void onAnalyticsScreenView( const ConstString & _screenType, const ConstString & _screenName ) override;

    protected:
        void onAnalyticsFlush() override;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<iOSAnalyticsEventProvider> iOSAnalyticsEventProviderPtr;
    //////////////////////////////////////////////////////////////////////////
}
