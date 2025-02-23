#pragma once

#include "Interface/ServiceInterface.h"

#include "Kernel/ConstString.h"
#include "Kernel/String.h"
#include "Kernel/Params.h"

namespace Mengine
{
    class AppleFirebaseRemoteConfigServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "AppleFirebaseRemoteConfigService" )

    public:
        virtual bool hasRemoteConfig( const ConstString & _key ) const = 0;
        virtual bool getRemoteConfigValue( const ConstString & _key, Params * const _params ) const = 0;
    };
}
//////////////////////////////////////////////////////////////////////////
#define APPLE_FIREBASE_REMOTECONFIG_SERVICE()\
    ((Mengine::AppleFirebaseRemoteConfigServiceInterface *)SERVICE_GET(Mengine::AppleFirebaseRemoteConfigServiceInterface))
//////////////////////////////////////////////////////////////////////////
