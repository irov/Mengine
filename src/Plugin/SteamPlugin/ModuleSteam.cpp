#   ifndef MENGINE_UNSUPPORT_PRAGMA_WARNING
#   pragma warning(push, 0) 
#   pragma warning(disable:4127)  
#   endif


#	include "ModuleSteam.h"

#	include "Logger/Logger.h"

//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	ModuleSteam::ModuleSteam()
		: m_client(nullptr)
		, m_user( nullptr )
		, m_userStats( nullptr )
		, m_friends( nullptr )
		, m_steamCallbackUserStatsReceived( this, &ModuleSteam::OnUserStatsReceived )
		, m_steamCallbackUserStatsStored( this, &ModuleSteam::OnUserStatsStored )
		, m_steamCallbackAchievementStored( this, &ModuleSteam::OnAchievementStored )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	ModuleSteam::~ModuleSteam()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	static ServiceProviderInterface * s_serviceProvider;
	//////////////////////////////////////////////////////////////////////////
	static void s_SteamAPIWarningMessageHook( int _severity, const char * _msg )
	{
		if( _severity >= 1 )
		{
			LOGGER_ERROR( s_serviceProvider )(_msg);
		}
		else
		{
			LOGGER_WARNING( s_serviceProvider )(_msg);
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool ModuleSteam::_initialize()
	{
		s_serviceProvider = m_serviceProvider;

		if( SteamAPI_RestartAppIfNecessary( k_uAppIdInvalid ) == true )
		{
			return false;
		}

		if( SteamAPI_Init() == false )
		{
			return false;
		}

		m_client = SteamClient();

		m_client->SetWarningMessageHook( &s_SteamAPIWarningMessageHook );

		HSteamPipe hSteamPipe = m_client->CreateSteamPipe();

		HSteamUser hSteamUser = m_client->ConnectToGlobalUser( hSteamPipe );

		m_user = m_client->GetISteamUser( hSteamUser, hSteamPipe, STEAMUSER_INTERFACE_VERSION );
		
		if( m_user->BLoggedOn() == false )
		{
			return false;
		}

		m_userStats = m_client->GetISteamUserStats( hSteamUser, hSteamPipe, STEAMUSERSTATS_INTERFACE_VERSION );

		SteamAPI_RunCallbacks();

		// mark it down
		//if( m_userStats->SetAchievement( "Winner" ) == false )
		//{
		//	return false;
		//}

		//if( m_userStats->GetStat() == false )
		//{
		//	return false;
		//}

		//if( m_userStats->GetAchievement() == false )
		//{
		//	return false;
		//}

		m_friends = m_client->GetISteamFriends( hSteamUser, hSteamPipe, STEAMFRIENDS_INTERFACE_VERSION );

		int friendcount = m_friends->GetFriendCount( k_EFriendFlagAll );

		for( int i = 0; i != friendcount; ++i )
		{
			CSteamID thisfriend = m_friends->GetFriendByIndex( i, k_EFriendFlagAll );

			const char * thisfriend_name = m_friends->GetFriendPersonaName( thisfriend );

			printf( "%s\n"
				, thisfriend_name
				);
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void ModuleSteam::_finalize()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void ModuleSteam::_update( float _time, float _timing )
	{
		(void)_time;
		(void)_timing;

		//bool status = m_userStats->RequestCurrentStats();
	}
	//////////////////////////////////////////////////////////////////////////
	void ModuleSteam::OnUserStatsReceived( UserStatsReceived_t *pCallback )
	{

	}
	//////////////////////////////////////////////////////////////////////////
	void ModuleSteam::OnUserStatsStored( UserStatsStored_t *pCallback )
	{

	}
	//////////////////////////////////////////////////////////////////////////
	void ModuleSteam::OnAchievementStored( UserAchievementStored_t *pCallback )
	{

	}
}

#   ifndef MENGINE_UNSUPPORT_PRAGMA_WARNING
#   pragma warning(pop) 
#   endif