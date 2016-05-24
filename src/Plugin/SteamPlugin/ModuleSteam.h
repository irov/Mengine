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

	//public:
	//	STEAM_CALLBACK( ModuleSteam, OnUserStatsReceived, UserStatsReceived_t, m_steamCallbackUserStatsReceived );
	//	STEAM_CALLBACK( ModuleSteam, OnUserStatsStored, UserStatsStored_t, m_steamCallbackUserStatsStored );
	//	STEAM_CALLBACK( ModuleSteam, OnAchievementStored, UserAchievementStored_t, m_steamCallbackAchievementStored );
	};
}