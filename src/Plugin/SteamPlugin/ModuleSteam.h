#pragma once

#include "Core/ModuleBase.h"

#include "steam/steam_api.h"
#include "steam/steam_gameserver.h"

namespace Mengine
{
	class ModuleSteam
		: public ModuleBase
	{
	public:
		ModuleSteam();
		~ModuleSteam();

	protected:
		bool _avaliable() override;

	protected:
		bool _initialize() override;
		void _finalize() override;

	protected:
		void _update( bool _focus ) override;

	protected:
		void _message( const ConstString & _messageName, const TMapWParams & _params ) override;

	protected:
		ISteamClient * m_client;
		ISteamUser * m_user;
		ISteamUserStats	* m_userStats;
		ISteamFriends * m_friends;

		typedef stdex::map<String, ConstString> TMapISO639_1;
		TMapISO639_1 m_iso639_1;
	};
}
