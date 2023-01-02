#pragma once

#include "AppleDevToDevInterface.h"

#include "Kernel/ServiceBase.h"

namespace Mengine
{
	class AppleDevToDevService
		: public ServiceBase<AppleDevToDevInterface>
	{
	public:
        AppleDevToDevService();
		~AppleDevToDevService() override;

    protected:
        bool _initializeService() override;
        void _finalizeService() override;

    protected:
        void sendEvent( NSString * _eventName, NSDictionary<NSString *, id> * parameters ) override;
    };
}
