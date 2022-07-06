#pragma once

#include "AppleFirebaseAnalyticsInterface.h"


#include "Kernel/ServiceBase.h"
#include "Kernel/VectorConstString.h"

namespace Mengine
{
	class AppleFirebaseAnalyticsService
		: public ServiceBase<AppleFirebaseAnalyticsServiceInterface>
	{
	public:
        AppleFirebaseAnalyticsService();
		~AppleFirebaseAnalyticsService() override;

    protected:
        bool _initializeService() override;
        void _finalizeService() override;
        
    public:
        bool sendEvent() override;
        
    };
}
