#pragma once

#include "Interface/TimeSystemInterface.h"

#include "Kernel/ServiceBase.h"

namespace Mengine
{
    class C11TimeSystem
        : public ServiceBase<TimeSystemInterface>
    {
    public:
        C11TimeSystem();
        ~C11TimeSystem() override;

    public:
        bool _initializeService() override;
        void _finalizeService() override;

    public:
        Timestamp getTimestamp() const override;
    };
};
