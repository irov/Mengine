#   pragma once

#   include "Interface/PlatformInterface.h"

#   include "s3eTimer.h"

namespace Menge
{
    class MarmaladeTimer
        : public TimerInterface
    {
    public:
        MarmaladeTimer();
        ~MarmaladeTimer();

    public:
        void initialize() override;
        void reset() override;

	public:
        float getDeltaTime() const override;
		uint64_t getUnixTime() const override;

    public:
        uint64_t getMilliseconds() override;
        uint64_t getMicroseconds() override;

    protected:
        mutable uint64 m_time;
        bool m_supportNanoseconds;
    };
}