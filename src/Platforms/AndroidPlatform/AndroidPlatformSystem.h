#pragma once

#include "Interface/PlatformSystemInterface.h"

#include "Kernel/ServiceBase.h"

namespace Mengine
{
    class AndroidPlatformSystem
        : public ServiceBase<PlatformSystemInterface>
    {
    public:
        AndroidPlatformSystem();
        ~AndroidPlatformSystem() override;

    public:
        bool _initializeService() override;
        void _finalizeService() override;
    };
}
