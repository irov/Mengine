#   ifndef MENGINE_UNSUPPORT_PRAGMA_WARNING
#   pragma warning(push, 0) 
#   pragma warning(disable:4127)  
#   endif

#include "ModuleSteam.h"

#include "Interface/ConfigInterface.h"
#include "Interface/OptionsInterface.h"
#include "Interface/StringizeInterface.h"
#include "Interface/UnicodeInterface.h"
#include "Interface/ApplicationInterface.h"

#include "Kernel/Logger.h"

//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    ModuleSteam::ModuleSteam()
        : m_client( nullptr )
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
    static void s_SteamAPIWarningMessageHook( int _severity, const char * _msg )
    {
        if( _severity >= 1 )
        {
            LOGGER_ERROR( "%s", _msg );
        }
        else
        {
            LOGGER_WARNING( "%s", _msg );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool ModuleSteam::_avaliable()
    {
        bool avaliable = CONFIG_VALUE( "Steam", "Avaliable", true );

        if( avaliable == false )
        {
            return false;
        }

        int32_t appId = CONFIG_VALUE( "Steam", "AppId", k_uAppIdInvalid );

        if( HAS_OPTION( "steamappid" ) == true )
        {
            const Char * str_steamappid = GET_OPTION_VALUE( "steamappid" );

            if( sscanf( str_steamappid, "%d", &appId ) != 0 )
            {
                LOGGER_ERROR( "ModuleSteam::_avaliable invalid option steamappid '%s'"
                    , str_steamappid
                );

                return false;
            }
        }

        if( HAS_OPTION( "norestartsteamapp" ) == false )
        {
            if( SteamAPI_RestartAppIfNecessary( appId ) == true )
            {
                LOGGER_ERROR( "ModuleSteam::_avaliable invalid SteamAPI_RestartAppIfNecessary [Id = %d]"
                    , appId
                );

                return false;
            }
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ModuleSteam::_initialize()
    {
        //danish, dutch, english, finnish, french, german, italian, korean,
        //norwegian, polish, portuguese, russian, schinese, spanish, swedish, tchinese, ukrainian

        //  brazilian
        //	bulgarian
        //	czech
        //	danish
        //	dutch
        //	english
        //	finnish
        //	french
        //	german
        //	greek
        //	hungarian
        //	italian
        //	japanese
        //	koreana
        //	norwegian
        //	polish
        //	portuguese
        //	romanian
        //	russian
        //	schinese
        //	spanish
        //	swedish
        //	tchinese
        //	thai
        //	turkish
        //	ukrainian

        m_iso639_1["brazilian"] = STRINGIZE_STRING_LOCAL( "pt-br" );
        m_iso639_1["bulgarian"] = STRINGIZE_STRING_LOCAL( "bg" );
        m_iso639_1["czech"] = STRINGIZE_STRING_LOCAL( "cs" );
        m_iso639_1["danish"] = STRINGIZE_STRING_LOCAL( "da" );
        m_iso639_1["dutch"] = STRINGIZE_STRING_LOCAL( "nl" );
        m_iso639_1["english"] = STRINGIZE_STRING_LOCAL( "en" );
        m_iso639_1["finnish"] = STRINGIZE_STRING_LOCAL( "fi" );
        m_iso639_1["french"] = STRINGIZE_STRING_LOCAL( "fr" );
        m_iso639_1["german"] = STRINGIZE_STRING_LOCAL( "de" );
        m_iso639_1["greek"] = STRINGIZE_STRING_LOCAL( "el" );
        m_iso639_1["hungarian"] = STRINGIZE_STRING_LOCAL( "hu" );
        m_iso639_1["italian"] = STRINGIZE_STRING_LOCAL( "it" );
        m_iso639_1["japanese"] = STRINGIZE_STRING_LOCAL( "ja" );
        m_iso639_1["koreana"] = STRINGIZE_STRING_LOCAL( "ko" );
        m_iso639_1["norwegian"] = STRINGIZE_STRING_LOCAL( "no" );
        m_iso639_1["polish"] = STRINGIZE_STRING_LOCAL( "pl" );
        m_iso639_1["portuguese"] = STRINGIZE_STRING_LOCAL( "pt" );
        m_iso639_1["romanian"] = STRINGIZE_STRING_LOCAL( "ro" );
        m_iso639_1["russian"] = STRINGIZE_STRING_LOCAL( "ru" );
        m_iso639_1["schinese"] = STRINGIZE_STRING_LOCAL( "zh" );
        m_iso639_1["spanish"] = STRINGIZE_STRING_LOCAL( "es" );
        m_iso639_1["swedish"] = STRINGIZE_STRING_LOCAL( "sv" );
        m_iso639_1["tchinese"] = STRINGIZE_STRING_LOCAL( "zh-tr" );
        m_iso639_1["thai"] = STRINGIZE_STRING_LOCAL( "th" );
        m_iso639_1["turkish"] = STRINGIZE_STRING_LOCAL( "tr" );
        m_iso639_1["ukrainian"] = STRINGIZE_STRING_LOCAL( "uk" );

        if( SteamAPI_Init() == false )
        {
            LOGGER_ERROR( "ModuleSteam::_initialize invalid SteamAPI_Init"
            );

            return false;
        }

        const char * AvailableGameLanguages = SteamApps()->GetAvailableGameLanguages();

        LOGGER_INFO( "ModuleSteam::initialize available game languages '%s'"
            , AvailableGameLanguages
        );

        const char * CurrentGameLanguage = SteamApps()->GetCurrentGameLanguage();

        LOGGER_WARNING( "ModuleSteam::initialize game language '%s'"
            , CurrentGameLanguage
        );

        MapISO639_1::const_iterator it_locale_found = m_iso639_1.find( CurrentGameLanguage );

        if( it_locale_found != m_iso639_1.end() )
        {
            const ConstString & steam_locale = it_locale_found->second;

            LOGGER_WARNING( "ModuleSteam::initialize found locale '%s' for language '%s'"
                , steam_locale.c_str()
                , CurrentGameLanguage
            );

            APPLICATION_SERVICE()
                ->setLocale( steam_locale );
        }
        else
        {
            LOGGER_ERROR( "ModuleSteam::initialize not found game localization for language '%s' setup 'en'"
                , CurrentGameLanguage
            );

            APPLICATION_SERVICE()
                ->setLocale( STRINGIZE_STRING_LOCAL( "en" ) );
        }

        m_client = SteamClient();

        m_client->SetWarningMessageHook( &s_SteamAPIWarningMessageHook );

        HSteamPipe hSteamPipe = m_client->CreateSteamPipe();

        HSteamUser hSteamUser = m_client->ConnectToGlobalUser( hSteamPipe );

        //m_user = m_client->GetISteamUser( hSteamUser, hSteamPipe, STEAMUSER_INTERFACE_VERSION );
        //
        //if( m_user->BLoggedOn() == false )
        //{
        //	LOGGER_ERROR("ModuleSteam::_initialize invalid BLoggedOn"
        //		);

        //	return false;
        //}

        m_userStats = m_client->GetISteamUserStats( hSteamUser, hSteamPipe, STEAMUSERSTATS_INTERFACE_VERSION );

        //m_userStats->RequestCurrentStats();
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
        m_userStats->StoreStats();

        SteamAPI_Shutdown();
    }
    //////////////////////////////////////////////////////////////////////////
    void ModuleSteam::_update( bool _focus )
    {
        (void)_focus;

        //bool status = m_userStats->RequestCurrentStats();
    }
    //////////////////////////////////////////////////////////////////////////
    void ModuleSteam::_message( const ConstString & _messageName, const MapWParams & _params )
    {
        m_userStats->RequestCurrentStats();

        if( _messageName == "SetAchievement" )
        {
            MapWParams::const_iterator it_value_found = _params.find( STRINGIZE_STRING_LOCAL( "Value" ) );

            if( it_value_found == _params.end() )
            {
                LOGGER_ERROR( "ModuleSteam::_message invalid message '%s' params [not found Value]"
                    , _messageName.c_str()
                );

                return;
            }

            const WString & unicode_value = it_value_found->second;

            String utf8_value;
            Helper::unicodeToUtf8( unicode_value, utf8_value );

            const char * str_value = utf8_value.c_str();

            if( m_userStats->SetAchievement( str_value ) == false )
            {
                LOGGER_ERROR( "ModuleSteam::_message invalid set achievement '%s'"
                    , str_value
                );

                return;
            }
        }
        else if( _messageName == "SetStat" )
        {
            MapWParams::const_iterator it_name_found = _params.find( STRINGIZE_STRING_LOCAL( "Name" ) );

            if( it_name_found == _params.end() )
            {
                LOGGER_ERROR( "ModuleSteam::_message invalid message '%s' params [not found Name]"
                    , _messageName.c_str()
                );

                return;
            }

            const WString & unicode_name = it_name_found->second;

            String utf8_name;
            Helper::unicodeToUtf8( unicode_name, utf8_name );

            const char * str_name = utf8_name.c_str();

            MapWParams::const_iterator it_value_found = _params.find( STRINGIZE_STRING_LOCAL( "Value" ) );

            if( it_value_found == _params.end() )
            {
                LOGGER_ERROR( "ModuleSteam::_message invalid message '%s' params [not found Value]"
                    , _messageName.c_str()
                );

                return;
            }

            const WString & unicode_value = it_value_found->second;

            String utf8_value;
            Helper::unicodeToUtf8( unicode_value, utf8_value );

            const char * str_value = utf8_value.c_str();

            MapWParams::const_iterator it_type_found = _params.find( STRINGIZE_STRING_LOCAL( "Type" ) );

            if( it_type_found == _params.end() )
            {
                LOGGER_ERROR( "ModuleSteam::_message invalid message '%s' params [not found Type]"
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
                    LOGGER_ERROR( "ModuleSteam::_message invalid stat '%s' float value '%s'"
                        , str_name
                        , str_value
                    );

                    return;
                }

                if( m_userStats->SetStat( str_name, f_value ) == false )
                {
                    LOGGER_ERROR( "ModuleSteam::_message invalid set stat '%s' float value '%f'"
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
                    LOGGER_ERROR( "ModuleSteam::_message invalid stat '%s' int value '%s'"
                        , str_name
                        , str_value
                    );

                    return;
                }

                if( m_userStats->SetStat( str_name, i_value ) == false )
                {
                    LOGGER_ERROR( "ModuleSteam::_message invalid set stat '%s' int value '%d'"
                        , str_value
                        , i_value
                    );

                    return;
                }
            }
            else
            {
                LOGGER_ERROR( "ModuleSteam::_message invalid set stat '%s' invalid type '%ls'"
                    , str_value
                    , unicode_type.c_str()
                );

                return;
            }
        }

        m_userStats->StoreStats();
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
