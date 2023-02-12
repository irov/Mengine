#pragma once

#include "Interface/ServiceInterface.h"

#import <Foundation/Foundation.h>

namespace Mengine
{
    class AppleFirebaseMessagingServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "AppleFirebaseMessagingService" )

    public:
        virtual void setPushToken( NSString * _pushToken ) = 0;
        virtual NSString * getPushToken() const = 0;
    };
}
//////////////////////////////////////////////////////////////////////////
#define APPLE_FIREBASE_MESSAGING_SERVICE()\
    ((Mengine::AppleFirebaseMessagingServiceInterface *)SERVICE_GET(Mengine::AppleFirebaseMessagingServiceInterface))
//////////////////////////////////////////////////////////////////////////
