#pragma once

#include "Interface/AnalyticsEventProviderInterface.h"

#include "Kernel/ConstString.h"
#include "Kernel/Factorable.h"

namespace Mengine
{
    class AppleFirebaseAnalyticsEventProvider
        : public AnalyticsEventProviderInterface
        , public Factorable
    {
        DECLARE_FACTORABLE( AppleFirebaseAnalyticsEventProvider );
        
    public:
        AppleFirebaseAnalyticsEventProvider();
        ~AppleFirebaseAnalyticsEventProvider() override;
        
    protected:
        void onAnalyticsEvent( const AnalyticsEventInterfacePtr & _event ) override;
        void onAnalyticsScreenView( const ConstString & _screenType, const ConstString & _screenName ) override;
        
        void onAnalyticsFlush() override;
    };
}
