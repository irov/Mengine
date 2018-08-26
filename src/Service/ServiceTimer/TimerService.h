#pragma once

#include "Interface/TimerInterface.h"

#include "Kernel/ServiceBase.h"

namespace Mengine
{
    class TimerService
        : public ServiceBase<TimerServiceInterface>
    {
    public:
        TimerService();
        ~TimerService() override;

    public:
        void resetDeltaTime() override;
        float getDeltaTime() override;

    public:
        uint64_t getMilliseconds() override;

    protected:
        uint64_t m_prevTime;
    };
};
