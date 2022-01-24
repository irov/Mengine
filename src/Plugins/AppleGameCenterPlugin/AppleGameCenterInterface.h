#pragma once

#include "Interface/ServiceInterface.h"

#include "Kernel/ConstString.h"

namespace Mengine
{
    class AppleGameCenterInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "AppleGameCenter" )

    public:
        virtual bool connect() = 0;

    public:
        virtual void reportAchievement( const ConstString & _achievementName, float _percentComplete ) = 0;
        virtual bool checkAchievement( const ConstString & _achievementName ) const = 0;

    public:
        virtual void reportScore( const ConstString & _key, uint32_t _score ) = 0;
    };
}
//////////////////////////////////////////////////////////////////////////
#define APPLE_GAMECENTER_SERVICE()\
    ((Mengine::AppleGameCenterInterface *)SERVICE_GET(Mengine::AppleGameCenterInterface))
//////////////////////////////////////////////////////////////////////////