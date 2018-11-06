#   pragma once

#   include "Interface/TimerInterface.h"

#   include "Core/ServiceBase.h"

namespace Mengine
{
    class SDLTimerSystem 
        : public ServiceBase<TimerSystemInterface>
    {
    public:
        SDLTimerSystem();
        ~SDLTimerSystem();

    public:
        bool _initialize() override;
        void _finalize() override;

    public:
        uint64_t getMilliseconds() override;
    };
};
