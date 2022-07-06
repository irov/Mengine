#pragma once

#include "AppleFirebaseCrashlyticsInterface.h"


#include "Kernel/ServiceBase.h"
#include "Kernel/VectorConstString.h"

namespace Mengine
{
	class AppleFirebaseCrashlyticsService
		: public ServiceBase<AppleFirebaseCrashlyticsServiceInterface>
	{
	public:
        AppleFirebaseCrashlyticsService();
		~AppleFirebaseCrashlyticsService() override;

    protected:
        bool _initializeService() override;
        void _finalizeService() override;
        
    public:
        bool sendEvent() override;
        
    };
}
