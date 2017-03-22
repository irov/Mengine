#   pragma once

#   include "Interface/TimerInterface.h"

#   include "Core/ServiceBase.h"

#   include <wtypes.h>

namespace Menge
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
