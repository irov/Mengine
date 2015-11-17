#   pragma once

#   include "Interface/TimerInterface.h"

#   include "s3eTimer.h"

namespace Menge
{
    class MarmaladeTimerSystem
		: public ServiceBase<TimerSystemInterface>
    {
    public:
		MarmaladeTimerSystem();
		~MarmaladeTimerSystem();

    public:
		uint64_t getMilliseconds() override;
    };
}