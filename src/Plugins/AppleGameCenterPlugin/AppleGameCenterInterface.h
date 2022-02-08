#pragma once

#include "Interface/ServiceInterface.h"
#include "Interface/Interface.h"

#include "Kernel/ConstString.h"

#include "Config/Lambda.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class AppleGameCenterProviderInterface
        : public Interface
    {
    public:
        virtual void onAppleGameConterAuthenticate( bool _successful ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<AppleGameCenterProviderInterface> AppleGameCenterProviderInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    class AppleGameCenterInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "AppleGameCenter" )

    public:
        virtual void setProvider( const AppleGameCenterProviderInterfacePtr & _provider ) = 0;
        virtual const AppleGameCenterProviderInterfacePtr & getProvider() const = 0;
        
    public:
        virtual bool connect() = 0;
        virtual bool isConnect() const = 0;

    public:
        typedef Lambda<void(bool, float)> LambdaAchievemtResponse;
        virtual bool reportAchievement( const ConstString & _achievementName, float _percentComplete, const LambdaAchievemtResponse & _response ) = 0;
        virtual bool checkAchievement( const ConstString & _achievementName ) const = 0;

    public:
        typedef Lambda<void(bool)> LambdaScoreResponse;
        virtual bool reportScore( const ConstString & _key, uint32_t _score, const LambdaScoreResponse & _response ) = 0;
    };
}
//////////////////////////////////////////////////////////////////////////
#define APPLE_GAMECENTER_SERVICE()\
    ((Mengine::AppleGameCenterInterface *)SERVICE_GET(Mengine::AppleGameCenterInterface))
//////////////////////////////////////////////////////////////////////////
