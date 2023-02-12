#pragma once

#include "Interface/ServiceInterface.h"

#import <Foundation/Foundation.h>

namespace Mengine
{
    class AppleFirebaseRemoteConfigServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "AppleFirebaseRemoteConfigService" )

    public:
        virtual void setRemoteConfig( NSMutableDictionary * _remoteConfig ) = 0;
        virtual NSMutableDictionary * getRemoteConfig() const = 0;
    };
}
//////////////////////////////////////////////////////////////////////////
#define APPLE_FIREBASE_REMOTECONFIG_SERVICE()\
    ((Mengine::AppleFirebaseRemoteConfigServiceInterface *)SERVICE_GET(Mengine::AppleFirebaseRemoteConfigServiceInterface))
//////////////////////////////////////////////////////////////////////////
