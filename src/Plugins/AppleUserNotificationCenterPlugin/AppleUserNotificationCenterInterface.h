#pragma once

#include "Interface/ServiceInterface.h"

#import <Foundation/Foundation.h>

namespace Mengine
{
    class AppleUserNotificationCenterServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "AppleUserNotificationCenterService" )

    public:
    };
}
//////////////////////////////////////////////////////////////////////////
#define APPLE_USERNOTIFICATIONCENTER_SERVICE()\
    ((Mengine::AppleUserNotificationCenterServiceInterface *)SERVICE_GET(Mengine::AppleUserNotificationCenterServiceInterface))
//////////////////////////////////////////////////////////////////////////
