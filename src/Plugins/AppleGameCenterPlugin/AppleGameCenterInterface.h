#pragma once

#include "Interface/ServiceInterface.h"

#include "Kernel/ConstString.h"

#include "Config/Lambda.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class AppleGameCenterProviderInterface
        : public Mixin
    {
    public:
        virtual void onAppleGameCenterAuthenticate( bool _successful ) = 0;
        virtual void onAppleGameCenterSynchronizate( bool _successful ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<AppleGameCenterProviderInterface> AppleGameCenterProviderInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    class AppleGameCenterServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "AppleGameCenterService" )

    public:
        virtual void setProvider( const AppleGameCenterProviderInterfacePtr & _provider ) = 0;
        virtual const AppleGameCenterProviderInterfacePtr & getProvider() const = 0;
        
    public:
        virtual bool connect() = 0;
        virtual bool isConnect() const = 0;
        virtual bool isSynchronizate() const = 0;

    public:
        typedef Lambda<void(bool)> LambdaAchievemtResponse;
        virtual bool reportAchievement( const ConstString & _achievementName, double _percentComplete, const LambdaAchievemtResponse & _response ) = 0;
        virtual bool checkAchievement( const ConstString & _achievementName ) const = 0;
        
    public:
        virtual bool resetAchievements() = 0;

    public:
        typedef Lambda<void(bool)> LambdaScoreResponse;
        virtual bool reportScore( const ConstString & _key, int64_t _score, const LambdaScoreResponse & _response ) = 0;
    };
}
//////////////////////////////////////////////////////////////////////////
#define APPLE_GAMECENTER_SERVICE()\
    ((Mengine::AppleGameCenterServiceInterface *)SERVICE_GET(Mengine::AppleGameCenterServiceInterface))
//////////////////////////////////////////////////////////////////////////
