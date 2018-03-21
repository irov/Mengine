#   pragma once

#include "Interface/TimerInterface.h"

#include "Core/ServiceBase.h"

namespace Mengine
{
    class C11TimerSystem 
        : public ServiceBase<TimerSystemInterface>
    {
    public:
        C11TimerSystem();
        ~C11TimerSystem();

    public:
        bool _initialize() override;
        void _finalize() override;

    public:
        uint64_t getMilliseconds() override;
    };
};
