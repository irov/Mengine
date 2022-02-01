#pragma once

#include "AppleAppTrackingInterface.h"

#include "Kernel/ServiceBase.h"

namespace Mengine
{
	class AppleAppTrackingService
		: public ServiceBase<AppleAppTrackingInterface>
	{
	public:
        AppleAppTrackingService();
		~AppleAppTrackingService() override;

    protected:
        bool _initializeService() override;
        void _finalizeService() override;
        
	protected:
		void authorization() override;
    };
}
