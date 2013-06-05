#   pragma once

#   include "Interface/ApplicationInterface.h"

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
        unsigned long getMilliseconds() override;
        unsigned long getMicroseconds() override;
        unsigned long getMillisecondsCPU() override;
        unsigned long getMicrosecondsCPU() override;

    protected:
        mutable uint64 m_time;
        bool m_supportNanoseconds;
    };
}