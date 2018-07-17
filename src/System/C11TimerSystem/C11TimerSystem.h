#pragma once

#include "Interface/TimerInterface.h"

#include "Kernel/ServiceBase.h"

namespace Mengine
{
    class C11TimerSystem 
        : public ServiceBase<TimerSystemInterface>
    {
    public:
        C11TimerSystem();
        ~C11TimerSystem() override;

    public:
        bool _initializeService() override;
        void _finalizeService() override;

    public:
        uint64_t getMilliseconds() override;
    };
};
