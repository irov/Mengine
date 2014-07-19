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
        float getDeltaTime() const override;

    public:
        uint64_t getMilliseconds() override;
        uint64_t getMicroseconds() override;
        uint64_t getMillisecondsCPU() override;
        uint64_t getMicrosecondsCPU() override;

    protected:
        mutable uint64 m_time;
        bool m_supportNanoseconds;
    };
}