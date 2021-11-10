#pragma once

#include "Interface/ServiceInterface.h"

namespace Mengine
{
    class SteamServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "SteamService" )

    public:
        virtual bool setAchievement( const ConstString & _name ) = 0;
        virtual bool getAchievement( const ConstString & _name, bool * const _exist ) const = 0;
        virtual bool setStateInteger( const ConstString & _name, int32_t _value ) = 0;
        virtual bool setStateFloat( const ConstString & _name, float _value ) = 0;
    };
}
//////////////////////////////////////////////////////////////////////////
#define STEAM_SERVICE()\
    ((Mengine::SteamServiceInterface *)SERVICE_GET(Mengine::SteamServiceInterface))
//////////////////////////////////////////////////////////////////////////
