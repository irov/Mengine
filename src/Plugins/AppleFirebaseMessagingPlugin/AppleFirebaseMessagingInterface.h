#pragma once

#include "Interface/ServiceInterface.h"

namespace Mengine
{
    class AppleFirebaseMessagingServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "AppleFirebaseCrashlyticsService" )

    public:
    };
}
//////////////////////////////////////////////////////////////////////////
#define APPLE_FIREBASE_MESSAGING_SERVICE()\
    ((Mengine::AppleFirebaseMessagingServiceInterface *)SERVICE_GET(Mengine::AppleFirebaseMessagingServiceInterface))
//////////////////////////////////////////////////////////////////////////
