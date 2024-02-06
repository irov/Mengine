#pragma once

#include "Interface/TimeSystemInterface.h"

#include "Kernel/ServiceBase.h"

namespace Mengine
{
    class Win32TimeSystem
        : public ServiceBase<TimeSystemInterface>
    {
    public:
        Win32TimeSystem();
        ~Win32TimeSystem() override;

    public:
        bool _initializeService() override;
        void _finalizeService() override;

    public:
        Timestamp getSystemTimestamp() const override;
    };
};
