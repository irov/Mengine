#pragma once

#include "AppleFirebaseMessagingInterface.h"

#include "Kernel/ServiceBase.h"
#include "Kernel/VectorConstString.h"

namespace Mengine
{
    class AppleFirebaseMessagingService
        : public ServiceBase<AppleFirebaseMessagingServiceInterface>
    {
    public:
        AppleFirebaseMessagingService();
        ~AppleFirebaseMessagingService() override;

    protected:
        bool _initializeService() override;
        void _finalizeService() override;

    public:
    };
}
