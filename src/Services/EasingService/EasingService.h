#pragma once

#include "Interface/EasingServiceInterface.h"

#include "Kernel/ServiceBase.h"

namespace Mengine
{
    class EasingService
        : public ServiceBase<EasingServiceInterface>
    {
        DECLARE_FACTORABLE( EasingService );

    public:
        EasingService();
        ~EasingService() override;

    public:
        bool _initializeService() override;
        void _finalizeService() override;
    };
}