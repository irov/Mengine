#pragma once

#include "Interface/PlatformSystemInterface.h"

#include "Kernel/ServiceBase.h"

namespace Mengine
{
    class Win32PlatformSystem
        : public ServiceBase<PlatformSystemInterface>
    {
    public:
        Win32PlatformSystem();
        ~Win32PlatformSystem() override;

    public:
        bool _initializeService() override;
        void _finalizeService() override;
    };
}
