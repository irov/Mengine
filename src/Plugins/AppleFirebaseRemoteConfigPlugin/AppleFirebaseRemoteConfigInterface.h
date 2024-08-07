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
        virtual bool getValueBoolean( const ConstString & _key ) const = 0;
        virtual int64_t getValueInteger( const ConstString & _key ) const = 0;
        virtual double getValueDouble( const ConstString & _key ) const = 0;
        virtual ConstString getValueConstString( const ConstString & _key ) const = 0;
        virtual Params getValueJSON( const ConstString & _key ) const = 0;
        
        virtual Params getValues() const = 0;
    };
}
//////////////////////////////////////////////////////////////////////////
#define APPLE_FIREBASE_REMOTECONFIG_SERVICE()\
    ((Mengine::AppleFirebaseRemoteConfigServiceInterface *)SERVICE_GET(Mengine::AppleFirebaseRemoteConfigServiceInterface))
//////////////////////////////////////////////////////////////////////////
