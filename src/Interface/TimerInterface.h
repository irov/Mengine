#   pragma once

#	include "Interface/ServiceInterface.h"

namespace Menge
{
    class TimerServiceInterface
		: public ServiceInterface
	{
		SERVICE_DECLARE( "TimerService" );

	public:
		virtual void resetDeltaTime() = 0;
        virtual float getDeltaTime() = 0;

	public:
        virtual uint64_t getMilliseconds() = 0;
    };

#   define TIMER_SERVICE( serviceProvider )\
    ((Menge::TimerServiceInterface*)SERVICE_GET(serviceProvider, Menge::TimerServiceInterface))

	class TimerSystemInterface
		: public ServiceInterface
	{
		SERVICE_DECLARE( "TimerSystem" );

	public:
		virtual uint64_t getMilliseconds() = 0;
	};

#   define TIMER_SYSTEM( serviceProvider )\
    ((Menge::TimerSystemInterface*)SERVICE_GET(serviceProvider, Menge::TimerSystemInterface))
}