#pragma once

#include "Interface/ServiceInterface.h"

namespace Mengine
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

#   define TIMER_SERVICE()\
    ((Mengine::TimerServiceInterface*)SERVICE_GET(Mengine::TimerServiceInterface))

	class TimerSystemInterface
		: public ServiceInterface
	{
		SERVICE_DECLARE( "TimerSystem" );

	public:
		virtual uint64_t getMilliseconds() = 0;
	};

#   define TIMER_SYSTEM()\
    ((Mengine::TimerSystemInterface*)SERVICE_GET(Mengine::TimerSystemInterface))
}