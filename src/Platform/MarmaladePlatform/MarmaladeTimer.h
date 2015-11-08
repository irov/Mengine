#   pragma once

#   include "Interface/TimerInterface.h"

#   include "s3eTimer.h"

namespace Menge
{
    class MarmaladeTimer
		: public ServiceBase<TimerSystemInterface>
    {
    public:
        MarmaladeTimer();
        ~MarmaladeTimer();

    public:
		uint64_t getMilliseconds() override;
    };
}