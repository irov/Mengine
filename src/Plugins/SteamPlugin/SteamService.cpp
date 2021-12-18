#ifndef MENGINE_UNSUPPORT_PRAGMA_WARNING
#   pragma warning(push, 0) 
#   pragma warning(disable:4127)
#endif

#include "SteamService.h"

#include "Interface/OptionsServiceInterface.h"
#include "Interface/StringizeServiceInterface.h"
#include "Interface/ApplicationInterface.h"
#include "Interface/PackageServiceInterface.h"
#include "Interface/NotificationServiceInterface.h"

#include "Kernel/Logger.h"
#include "Kernel/UnicodeHelper.h"
#include "Kernel/ConfigHelper.h"

#include "Config/StdIO.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( SteamService, Mengine::SteamService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        static void SteamAPIWarningMessageHook( int _severity, const char * _msg )
        {
            if( _severity >= 1 )
            {
                LOGGER_ERROR( "[steam] %s", _msg );
            }
            else
            {
                LOGGER_WARNING( "[steam] %s", _msg );
            }
        }
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
    SteamService::SteamService()
        : m_client( nullptr )
        , m_user( nullptr )
        , m_userStats( nullptr )
        , m_friends( nullptr )
        , m_steamInitialize( false )
        //, m_steamCallbackUserStatsReceived( this, &ModuleSteam::OnUserStatsReceived )
        //, m_steamCallbackUserStatsStored( this, &ModuleSteam::OnUserStatsStored )
        //, m_steamCallbackAchievementStored( this, &ModuleSteam::OnAchievementStored )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    SteamService::~SteamService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool SteamService::_initializeService()
    {
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
        m_iso639_1["ukrainian"] = STRINGIZE_STRING_LOCAL( "ua" );

        if( SteamAPI_Init() == false )
        {
            LOGGER_ERROR( "[steam] invalid SteamAPI_Init" );

#ifdef MENGINE_DEBUG
            return true;
#else
            return false;
#endif
        }

        m_steamInitialize = true;

        NOTIFICATION_ADDOBSERVERMETHOD_THIS( NOTIFICATOR_PACKAGES_LOAD, &SteamService::notifyPackagesLoad_, MENGINE_DOCUMENT_FACTORABLE );

        ISteamClient * client = SteamClient();

        if( client == nullptr )
        {
            LOGGER_ERROR( "[steam] invalid SteamClient" );

            return false;
        }

        m_client = client;

        m_client->SetWarningMessageHook( &Detail::SteamAPIWarningMessageHook );

        HSteamPipe hSteamPipe = m_client->CreateSteamPipe();

        HSteamUser hSteamUser = m_client->ConnectToGlobalUser( hSteamPipe );

        ISteamUser * user = SteamUser();

        if( user == nullptr )
        {
            LOGGER_ERROR( "[steam] invalid SteamUser" );

            return false;
        }

        if( user->BLoggedOn() == false )
        {
            LOGGER_ERROR( "[steam] user is not logged in" );

            return false;
        }

        //m_user = m_client->GetISteamUser( hSteamUser, hSteamPipe, STEAMUSER_INTERFACE_VERSION );
        //
        //if( m_user->BLoggedOn() == false )
        //{
        //	LOGGER_ERROR("invalid BLoggedOn"
        //		);

        //	return false;
        //}

        ISteamUserStats * userStats = m_client->GetISteamUserStats( hSteamUser, hSteamPipe, STEAMUSERSTATS_INTERFACE_VERSION );

        if( userStats == nullptr )
        {
            LOGGER_ERROR( "[steam] invalid get UserStats" );

            return false;
        }

        m_userStats = userStats;

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
    void SteamService::_finalizeService()
    {
        if( m_steamInitialize == false )
        {
            return;
        }

        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_PACKAGES_LOAD );

        m_userStats->StoreStats();

        SteamAPI_Shutdown();
    }
    //////////////////////////////////////////////////////////////////////////
    bool SteamService::setAchievement( const ConstString & _name )
    {
        if( m_steamInitialize == false )
        {
            LOGGER_MESSAGE( "Steam not initialize" );

            return false;
        }

        m_userStats->RequestCurrentStats();

        if( m_userStats->SetAchievement( _name.c_str() ) == false )
        {
            LOGGER_ERROR( "invalid set achievement '%s'"
                , _name.c_str()
            );

            return false;
        }

        m_userStats->StoreStats();

        LOGGER_MESSAGE( "Steam set achievement '%s'"
            , _name.c_str()
        );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SteamService::getAchievement( const ConstString & _name, bool * const _achieved ) const
    {
        if( m_steamInitialize == false )
        {
            LOGGER_MESSAGE( "Steam not initialize" );

            return false;
        }

        m_userStats->RequestCurrentStats();

        if( m_userStats->GetAchievement( _name.c_str(), _achieved ) == false )
        {
            LOGGER_ERROR( "invalid get achievement '%s'"
                , _name.c_str()
            );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SteamService::setStateInteger( const ConstString & _name, int32_t _value )
    {
        if( m_steamInitialize == false )
        {
            LOGGER_MESSAGE( "Steam not initialize" );

            return false;
        }

        m_userStats->RequestCurrentStats();

        if( m_userStats->SetStat( _name.c_str(), _value ) == false )
        {
            LOGGER_ERROR( "invalid set stat '%s' int32_t value '%d'"
                , _name.c_str()
                , _value
            );

            return false;
        }

        m_userStats->StoreStats();

        LOGGER_MESSAGE( "Steam set state '%s' value '%u'"
            , _name.c_str()
            , _value
        );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SteamService::setStateFloat( const ConstString & _name, float _value )
    {
        if( m_steamInitialize == false )
        {
            LOGGER_MESSAGE( "Steam not initialize" );

            return false;
        }

        m_userStats->RequestCurrentStats();

        if( m_userStats->SetStat( _name.c_str(), _value ) == false )
        {
            LOGGER_ERROR( "invalid set stat '%s' int32_t value '%f'"
                , _name.c_str()
                , _value
            );

            return false;
        }

        m_userStats->StoreStats();

        LOGGER_MESSAGE( "Steam set state '%s' value '%f'"
            , _name.c_str()
            , _value
        );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void SteamService::notifyPackagesLoad_()
    {
        if( m_steamInitialize == false )
        {
            return;
        }

        ISteamApps * apps = SteamApps();

        if( apps == nullptr )
        {
            LOGGER_ERROR( "SteamApps not initalize [nullptr]" );

            return;
        }

        const Char * AvailableGameLanguages = apps->GetAvailableGameLanguages();

        if( AvailableGameLanguages == nullptr )
        {
            LOGGER_ERROR( "AvailableGameLanguages return [nullptr]" );

            return;
        }

        LOGGER_MESSAGE( "available game languages: %s"
            , AvailableGameLanguages
        );

        const Char * CurrentGameLanguage = apps->GetCurrentGameLanguage();

        if( CurrentGameLanguage == nullptr )
        {
            LOGGER_ERROR( "CurrentGameLanguage return [nullptr]" );

            return;
        }

        LOGGER_MESSAGE( "steam game language: %s"
            , CurrentGameLanguage
        );

        MapISO639_1::const_iterator it_locale_found = m_iso639_1.find( CurrentGameLanguage );

        if( it_locale_found == m_iso639_1.end() )
        {
            const ConstString & locale = APPLICATION_SERVICE()
                ->getLocale();

            LOGGER_MESSAGE( "not found game localization for language '%s' stay current localization '%s'"
                , CurrentGameLanguage
                , locale.c_str()
            );

            return;
        }

        const ConstString & steam_locale = it_locale_found->second;

        LOGGER_WARNING( "found locale '%s' for language '%s'"
            , steam_locale.c_str()
            , CurrentGameLanguage
        );

        if( PACKAGE_SERVICE()
            ->existLocalePackage( steam_locale, {} ) == false )
        {
            LOGGER_MESSAGE( "not found game localization for language '%s'"
                , CurrentGameLanguage
            );

            return;
        }

        APPLICATION_SERVICE()
            ->setLocale( steam_locale );
    }
    //////////////////////////////////////////////////////////////////////////
}

#ifndef MENGINE_UNSUPPORT_PRAGMA_WARNING
#   pragma warning(pop) 
#endif