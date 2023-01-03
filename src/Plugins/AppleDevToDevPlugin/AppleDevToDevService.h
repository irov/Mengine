#pragma once

#include "AppleDevToDevInterface.h"

#include "Interface/AnalyticsServiceInterface.h"

#include "Kernel/ServiceBase.h"

namespace Mengine
{
	class AppleDevToDevService
		: public ServiceBase<AppleDevToDevInterface>
        , public AnalyticsEventProviderInterface
	{
	public:
        AppleDevToDevService();
		~AppleDevToDevService() override;

    protected:
        bool _initializeService() override;
        void _finalizeService() override;

    protected:
        void sendEvent( NSString * _eventName, NSDictionary<NSString *, id> * _parameters ) override;
        
    protected:
        void onAnalyticsEvent( const AnalyticsEventInterfacePtr & _event ) override;
    };
}
