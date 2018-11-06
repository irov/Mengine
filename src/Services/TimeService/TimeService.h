#pragma once

#include "Interface/TimeServiceInterface.h"

#include "Kernel/ServiceBase.h"

namespace Mengine
{
    class TimeService
        : public ServiceBase<TimeServiceInterface>
    {
    public:
        TimeService();
        ~TimeService() override;

    public:
        void resetDeltaTime() override;
        float getDeltaTime() override;

    protected:
        uint64_t m_prevTime;
    };
};
