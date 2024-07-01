#pragma once

#include "AppleFirebaseAnalyticsInterface.h"

#include "Interface/AnalyticsServiceInterface.h"

#include "Kernel/ServiceBase.h"
#include "Kernel/VectorConstString.h"
#include "Kernel/Map.h"

namespace Mengine
{
    class AppleFirebaseAnalyticsService
        : public ServiceBase<AppleFirebaseAnalyticsServiceInterface>
        , public AnalyticsEventProviderInterface
    {
    public:
        AppleFirebaseAnalyticsService();
        ~AppleFirebaseAnalyticsService() override;

    protected:
        bool _initializeService() override;
        void _finalizeService() override;

    public:
        void sendEvent( NSString * _name, NSDictionary<NSString *, id> * parameters ) override;
        
    protected:
        void onAnalyticsEvent( const AnalyticsEventInterfacePtr & _event ) override;
        void onAnalyticsScreenView( const ConstString & _screenType, const ConstString & _screenName ) override;
        
        void onAnalyticsFlush() override;
    };
}
