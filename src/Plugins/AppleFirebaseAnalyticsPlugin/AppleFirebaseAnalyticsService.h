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
        NSString * getkFIREventAdImpression() const override;
        NSString * getkFIRParameterAdPlatform() const override;
        NSString * getkFIRParameterAdSource() const override;
        NSString * getkFIRParameterAdFormat() const override;
        NSString * getkFIRParameterAdUnitName() const override;
        NSString * getkFIRParameterCurrency() const override;
        NSString * getkFIRParameterValue() const override;

    public:
        void sendEvent( NSString * _name, NSDictionary<NSString *, id> * parameters ) override;
        
    protected:
        void onAnalyticsEvent( const AnalyticsEventInterfacePtr & _event ) override;
        void onAnalyticsFlush() override;
    };
}
