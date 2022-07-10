#pragma once

#include "Interface/ServiceInterface.h"
#include "Interface/ServantInterface.h"

#include "Kernel/Map.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    typedef Map<ConstString, ConstString> FirebaseAnalyticsParams;
    //////////////////////////////////////////////////////////////////////////
    class AppleFirebaseAnalyticsServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "AppleFirebaseAnalyticsService" )

    public:
        virtual void sendEvent( const ConstString & _name, const FirebaseAnalyticsParams & _params ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
}
//////////////////////////////////////////////////////////////////////////
#define APPLE_FIREBASE_ANALYTICS_SERVICE()\
    ((Mengine::AppleFirebaseAnalyticsServiceInterface *)SERVICE_GET(Mengine::AppleFirebaseAnalyticsServiceInterface))
//////////////////////////////////////////////////////////////////////////
