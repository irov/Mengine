#pragma once

#include "AppleAppTrackingInterface.h"

#include "Kernel/ServiceBase.h"
#include "Kernel/VectorConstString.h"

namespace Mengine
{
	class AppleAppTrackingService
		: public ServiceBase<AppleAppTrackingInterface>
	{
	public:
		AppleGameCenterService();
		~AppleGameCenterService() override;

    protected:
        bool _initializeService() override;
        void _finalizeService() override;
        
	protected:
		void authorization() override;
    };
}
