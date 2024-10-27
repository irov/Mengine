#pragma once

#include "Interface/iOSKernelServiceInterface.h"
#include "Interface/LoggerInterface.h"

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
