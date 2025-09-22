#pragma once

#import "Environment/iOS/iOSKernelServiceInterface.h"

#include "Kernel/ServiceBase.h"

namespace Mengine
{
    class iOSKernelService
        : public ServiceBase<iOSKernelServiceInterface>
    {
    public:
        iOSKernelService();
        ~iOSKernelService() override;

    public:
        bool _initializeService() override;
        void _finalizeService() override;
    };
}
