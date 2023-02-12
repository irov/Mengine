#pragma once

#include "AppleUserNotificationCenterInterface.h"

#include "Kernel/ServiceBase.h"

namespace Mengine
{
    class AppleUserNotificationCenterService
        : public ServiceBase<AppleUserNotificationCenterServiceInterface>
    {
    public:
        AppleUserNotificationCenterService();
        ~AppleUserNotificationCenterService() override;

    protected:
        bool _initializeService() override;
        void _finalizeService() override;

    public:
    };
}
