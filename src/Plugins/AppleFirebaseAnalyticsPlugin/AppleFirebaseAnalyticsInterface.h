#pragma once

#include "Interface/ServiceInterface.h"

#import <Foundation/Foundation.h>

namespace Mengine
{
    class AppleFirebaseAnalyticsServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "AppleFirebaseAnalyticsService" )
        
    public:
        virtual void sendEvent( NSString * _name, NSDictionary<NSString *, id> * parameters ) = 0;
    };
}
//////////////////////////////////////////////////////////////////////////
#define APPLE_FIREBASE_ANALYTICS_SERVICE()\
    ((Mengine::AppleFirebaseAnalyticsServiceInterface *)SERVICE_GET(Mengine::AppleFirebaseAnalyticsServiceInterface))
//////////////////////////////////////////////////////////////////////////
