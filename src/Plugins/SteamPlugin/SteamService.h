#pragma once

#include "SteamServiceInterface.h"

#include "Kernel/ServiceBase.h"
#include "Kernel/String.h"
#include "Kernel/ConstString.h"
#include "Kernel/Map.h"

#include "steam/steam_api.h"
#include "steam/steam_gameserver.h"

namespace Mengine
{
    class SteamService
        : public ServiceBase<SteamServiceInterface>
    {
    public:
        SteamService();
        ~SteamService() override;

    public:
        bool _initializeService() override;
        void _finalizeService() override;

    protected:
        bool setAchievement( const ConstString & _name ) override;
        bool getAchievement( const ConstString & _name, bool * const _exist ) const override;
        bool setStateInteger( const ConstString & _name, int32_t _value ) override;
        bool setStateFloat( const ConstString & _name, float _value ) override;

    protected:
        void notifyPackagesLoad_();

    protected:
        ISteamClient * m_client;
        ISteamUser * m_user;
        ISteamUserStats	* m_userStats;
        ISteamFriends * m_friends;

        typedef Map<String, ConstString> MapISO639_1;
        MapISO639_1 m_iso639_1;

        bool m_steamInitialize;
    };
}
