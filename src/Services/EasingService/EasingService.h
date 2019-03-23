#pragma once

#include "Interface/EasingServiceInterface.h"

#include "Kernel/ServiceBase.h"

namespace Mengine
{
    class EasingService
        : public ServiceBase<EasingServiceInterface>
    {
    public:
        EasingService();
        ~EasingService() override;

    public:
        bool _initializeService() override;
        void _finalizeService() override;
    };
}