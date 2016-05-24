#   ifndef MENGINE_UNSUPPORT_PRAGMA_WARNING
#   pragma warning(push, 0) 
#   pragma warning(disable:4127)  
#   endif

#	include "ModuleSteam.h"

#	include "Interface/ConfigInterface.h"
#	include "Interface/StringizeInterface.h"
#	include "Interface/UnicodeInterface.h"

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
		//, m_steamCallbackUserStatsReceived( this, &ModuleSteam::OnUserStatsReceived )
		//, m_steamCallbackUserStatsStored( this, &ModuleSteam::OnUserStatsStored )
		//, m_steamCallbackAchievementStored( this, &ModuleSteam::OnAchievementStored )
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
		
#	ifndef _DEBUG
		int32_t appId = CONFIG_VALUE( m_serviceProvider, "Steam", "AppId", k_uAppIdInvalid );

		if( SteamAPI_RestartAppIfNecessary( appId ) == true )
		{
			return false;
		}
#	endif

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

		m_userStats->RequestCurrentStats();
		//m_userStats->RequestCurrentStats();
				
		//if( m_userStats->GetStat() == false )
		//{
		//	return false;
		//}

		//if( m_userStats->GetAchievement() == false )
		//{
		//	return false;
		//}

		//m_friends = m_client->GetISteamFriends( hSteamUser, hSteamPipe, STEAMFRIENDS_INTERFACE_VERSION );

		//int friendcount = m_friends->GetFriendCount( k_EFriendFlagAll );

		//for( int i = 0; i != friendcount; ++i )
		//{
		//	CSteamID thisfriend = m_friends->GetFriendByIndex( i, k_EFriendFlagAll );

		//	const char * thisfriend_name = m_friends->GetFriendPersonaName( thisfriend );

		//	printf( "%s\n"
		//		, thisfriend_name
		//		);
		//}

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
	void ModuleSteam::_message( const ConstString & _messageName, const TMapParams & _params )
	{
		m_userStats->RequestCurrentStats();

		if( _messageName == "SetAchievement" )
		{
			TMapParams::const_iterator it_value_found = _params.find( STRINGIZE_STRING_LOCAL( m_serviceProvider, "Value" ) );

			if( it_value_found == _params.end() )
			{
				LOGGER_ERROR( m_serviceProvider )("ModuleSteam::_message invalid message '%s' params [not found Value]"
					, _messageName.c_str()
					);

				return;
			}

			const WString & unicode_value = it_value_found->second;

			String utf8_value;
			Helper::unicodeToUtf8( m_serviceProvider, unicode_value, utf8_value );

			const char * str_value = utf8_value.c_str();

			if( m_userStats->SetAchievement( str_value ) == false )
			{
				LOGGER_ERROR( m_serviceProvider )("ModuleSteam::_message invalid set achievement '%s'"
					, str_value
					);

				return;
			}
		}
		else if( _messageName == "SetStat" )
		{
			TMapParams::const_iterator it_name_found = _params.find( STRINGIZE_STRING_LOCAL( m_serviceProvider, "Name" ) );

			if( it_name_found == _params.end() )
			{
				LOGGER_ERROR( m_serviceProvider )("ModuleSteam::_message invalid message '%s' params [not found Name]"
					, _messageName.c_str()
					);

				return;
			}

			const WString & unicode_name = it_name_found->second;

			String utf8_name;
			Helper::unicodeToUtf8( m_serviceProvider, unicode_name, utf8_name );

			const char * str_name = utf8_name.c_str();

			TMapParams::const_iterator it_value_found = _params.find( STRINGIZE_STRING_LOCAL( m_serviceProvider, "Value" ) );

			if( it_value_found == _params.end() )
			{
				LOGGER_ERROR( m_serviceProvider )("ModuleSteam::_message invalid message '%s' params [not found Value]"
					, _messageName.c_str()
					);

				return;
			}

			const WString & unicode_value = it_value_found->second;

			String utf8_value;
			Helper::unicodeToUtf8( m_serviceProvider, unicode_value, utf8_value );

			const char * str_value = utf8_value.c_str();

			TMapParams::const_iterator it_type_found = _params.find( STRINGIZE_STRING_LOCAL( m_serviceProvider, "Type" ) );

			if( it_type_found == _params.end() )
			{
				LOGGER_ERROR( m_serviceProvider )("ModuleSteam::_message invalid message '%s' params [not found Type]"
					, _messageName.c_str()
					);

				return;
			}

			const WString & unicode_type = it_type_found->second;
			
			if( unicode_type == L"float" )
			{
				float f_value;
				if( sscanf( str_value, "%f", &f_value ) != 1 )
				{
					LOGGER_ERROR( m_serviceProvider )("ModuleSteam::_message invalid stat '%s' float value '%s'"
						, str_name
						, str_value
						);

					return;
				}

				if( m_userStats->SetStat( str_name, f_value ) == false )
				{
					LOGGER_ERROR( m_serviceProvider )("ModuleSteam::_message invalid set stat '%s' float value '%f'"
						, str_value
						, f_value
						);

					return;
				}
			}
			else if( unicode_type == L"int" )
			{
				int32_t i_value;
				if( sscanf( str_value, "%d", &i_value ) != 1 )
				{
					LOGGER_ERROR( m_serviceProvider )("ModuleSteam::_message invalid stat '%s' int value '%s'"
						, str_name
						, str_value
						);

					return;
				}

				if( m_userStats->SetStat( str_name, i_value ) == false )
				{
					LOGGER_ERROR( m_serviceProvider )("ModuleSteam::_message invalid set stat '%s' int value '%d'"
						, str_value
						, i_value
						);

					return;
				}
			}
			else
			{
				LOGGER_ERROR( m_serviceProvider )("ModuleSteam::_message invalid set stat '%s' invalid type '%ls'"
					, str_value
					, unicode_type.c_str()
					);

				return;
			}
		}
	}
	////////////////////////////////////////////////////////////////////////////
	//void ModuleSteam::OnUserStatsReceived( UserStatsReceived_t * _pCallback )
	//{

	//}
	////////////////////////////////////////////////////////////////////////////
	//void ModuleSteam::OnUserStatsStored( UserStatsStored_t * _pCallback )
	//{

	//}
	////////////////////////////////////////////////////////////////////////////
	//void ModuleSteam::OnAchievementStored( UserAchievementStored_t * _pCallback )
	//{

	//}
}

#   ifndef MENGINE_UNSUPPORT_PRAGMA_WARNING
#   pragma warning(pop) 
#   endif