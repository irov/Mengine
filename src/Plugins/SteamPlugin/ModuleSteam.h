#pragma once

#include "Kernel/ModuleBase.h"

#include "steam/steam_api.h"
#include "steam/steam_gameserver.h"

namespace Mengine
{
    class ModuleSteam
        : public ModuleBase
    {
        DECLARE_FACTORABLE( ModuleSteam );

    public:
        ModuleSteam();
        ~ModuleSteam() override;

    protected:
        bool _availableModule() const override;

    protected:
        bool _initializeModule() override;
        void _finalizeModule() override;

    protected:
        void _update( bool _focus ) override;

    protected:
        void _message( const ConstString & _messageName, const MapWParams & _params ) override;

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
