#pragma once

#include "Interface/AnalyticsEventProviderInterface.h"

#include "Kernel/ConstString.h"
#include "Kernel/Factorable.h"

namespace Mengine
{
    class AppleAmplitudeAnalyticsEventProvider
        : public AnalyticsEventProviderInterface
        , public Factorable
    {
        DECLARE_FACTORABLE( AppleAmplitudeAnalyticsEventProvider );

    public:
        AppleAmplitudeAnalyticsEventProvider();
        ~AppleAmplitudeAnalyticsEventProvider() override;

    protected:
        void onAnalyticsEvent( const AnalyticsEventInterfacePtr & _event ) override;
        void onAnalyticsScreenView( const ConstString & _screenType, const ConstString & _screenName ) override;
        void onAnalyticsFlush() override;
    };
}

