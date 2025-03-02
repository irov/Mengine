#pragma once

#include "Interface/TimeSystemInterface.h"

#include "Kernel/ServiceBase.h"

namespace Mengine
{
    class POSIXTimeSystem
        : public ServiceBase<TimeSystemInterface>
    {
    public:
        POSIXTimeSystem();
        ~POSIXTimeSystem() override;

    public:
        bool _initializeService() override;
        void _finalizeService() override;

    public:
        Timestamp getSystemTimestamp() const override;

    public:
        double getElapsedTime() const override;
    };
};
