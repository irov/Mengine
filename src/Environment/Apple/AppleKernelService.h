#pragma once

#import "Environment/Apple/AppleKernelServiceInterface.h"

#include "Kernel/ServiceBase.h"

namespace Mengine
{
    class AppleKernelService
        : public ServiceBase<AppleKernelServiceInterface>
    {
    public:
        AppleKernelService();
        ~AppleKernelService() override;

    public:
        bool _initializeService() override;
        void _finalizeService() override;
    };
}
