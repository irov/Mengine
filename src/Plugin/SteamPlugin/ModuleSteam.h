#	pragma once

#	include "Core/ModuleBase.h"

#	include "steam/steam_api.h"
#	include "steam/steam_gameserver.h"

namespace Menge
{
	class ModuleSteam
		: public ModuleBase
	{
	public:
		ModuleSteam();
		~ModuleSteam();

	protected:
		bool _initialize() override;
		void _finalize() override;

	protected:
		void _update( float _time, float _timing ) override;

	protected:
		void _message( const ConstString & _messageName, const TMapParams & _params ) override;

	protected:
		ISteamClient * m_client;
		ISteamUser * m_user;
		ISteamUserStats	* m_userStats;
		ISteamFriends * m_friends;

		typedef stdex::map<String, ConstString> TMapISO639_1;
		TMapISO639_1 m_iso639_1;
	};
}