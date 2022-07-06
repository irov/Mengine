#pragma once

#include "Interface/ServiceInterface.h"
#include "Interface/ServantInterface.h"

namespace Mengine
{
   
  //////////////////////////////////////////////////////////////////////////
    class AppleFirebaseAnalyticsServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "AppleFirebaseAnalyticsService" )

    public:
        virtual bool sendEvent() = 0;
        
    };
    //////////////////////////////////////////////////////////////////////////
}
//////////////////////////////////////////////////////////////////////////
#define APPLE_FIREBASE_ANALYTICS_SERVICE()\
    ((Mengine::AppleFirebaseAnalyticsServiceInterface *)SERVICE_GET(Mengine::AppleFirebaseAnalyticsServiceInterface))
//////////////////////////////////////////////////////////////////////////
