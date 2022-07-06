#pragma once

#include "Interface/ServiceInterface.h"
#include "Interface/ServantInterface.h"

namespace Mengine
{
   
  //////////////////////////////////////////////////////////////////////////
    class AppleFirebaseCrashlyticsServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "AppleFirebaseCrashlyticsService" )

    public:
        virtual bool sendEvent() = 0;
        
    };
    //////////////////////////////////////////////////////////////////////////
}
//////////////////////////////////////////////////////////////////////////
#define APPLE_FIREBASE_CRASHLYTICS_SERVICE()\
    ((Mengine::AppleFirebaseCrashlyticsServiceInterface *)SERVICE_GET(Mengine::AppleFirebaseCrashlyticsServiceInterface))
//////////////////////////////////////////////////////////////////////////
