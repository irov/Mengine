#pragma once

#include "Interface/ServiceInterface.h"

#include "Config/Char.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    enum EAppleAppTrackingAuthorization
    {
        EAATA_NONE,
        EAATA_AUTHORIZED,
        EAATA_DENIED,
        EAATA_RESTRICTED,
        EAATA_NOT_DETERMINED
    };
    //////////////////////////////////////////////////////////////////////////
    class AppleAppTrackingInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "AppleAppTracking" )

    public:
        typedef Lambda<void(EAppleAppTrackingAuthorization _status, const Char * _idfa)> LambdaAuthorizationResponse;
        virtual void authorization( const LambdaAuthorizationResponse & _response ) = 0;
        
    public:
        virtual void getIDFA( EAppleAppTrackingAuthorization * const _status, Char * const _idfa ) const = 0;

    public:
        virtual bool isTrackingAllowed() const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
}
//////////////////////////////////////////////////////////////////////////
#define APPLE_APPTRACKING_SERVICE()\
    ((Mengine::AppleAppTrackingInterface *)SERVICE_GET(Mengine::AppleAppTrackingInterface))
//////////////////////////////////////////////////////////////////////////
