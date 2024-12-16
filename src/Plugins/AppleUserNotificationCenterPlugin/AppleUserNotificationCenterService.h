#pragma once

#include "AppleUserNotificationCenterInterface.h"

#include "Kernel/ServiceBase.h"

#include "Config/Timestamp.h"

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
        void scheduleNotification( int32_t _badge, NSString * _title, NSString * _body, Timestamp _delay );
        
    protected:
        BOOL m_notificationPermissionGranted;
    };
}
