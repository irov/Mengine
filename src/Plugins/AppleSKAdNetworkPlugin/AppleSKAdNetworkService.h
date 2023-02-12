#pragma once

#include "AppleSKAdNetworkInterface.h"

#include "Kernel/ServiceBase.h"

namespace Mengine
{
    class AppleSKAdNetworkService
        : public ServiceBase<AppleSKAdNetworkServiceInterface>
    {
    public:
        AppleSKAdNetworkService();
        ~AppleSKAdNetworkService() override;

    protected:
        bool _initializeService() override;
        void _finalizeService() override;

    public:
    };
}
