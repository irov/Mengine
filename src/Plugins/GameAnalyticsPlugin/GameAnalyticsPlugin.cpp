#include "GameAnalyticsPlugin.h"

#include "Interface/PlatformInterface.h"
#include "Interface/OptionsServiceInterface.h"
#include "Interface/ConfigServiceInterface.h"
#include "Interface/FileServiceInterface.h"
#include "Interface/NotificationServiceInterface.h"
#include "Interface/AccountServiceInterface.h"

#include "GameAnalytics.h"

#include "Kernel/Logger.h"
#include "Kernel/AssertionAllocator.h"
#include "Kernel/FilePathDateTimeHelper.h"
#include "Kernel/ConfigHelper.h"
#include "Kernel/BuildMode.h"
#include "Kernel/ArrayString.h"

#include "Config/StdString.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_EXTERN( AnalyticsSystem );
//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( GameAnalytics, Mengine::GameAnalyticsPlugin );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    GameAnalyticsPlugin::GameAnalyticsPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    GameAnalyticsPlugin::~GameAnalyticsPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool GameAnalyticsPlugin::_availablePlugin() const
    {
        bool Engine_VideoPluginAvailable = CONFIG_VALUE( "Engine", "GameAnalyticsPluginAvailable", true );

        if( Engine_VideoPluginAvailable == false )
        {
            return false;
        }

        bool Engine_Analytics = CONFIG_VALUE( "Engine", "Analytics", true );

        if( Engine_Analytics == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool GameAnalyticsPlugin::_initializePlugin()
    {
        const Char * GameAnalytics_GameKey = CONFIG_VALUE( "GameAnalytics", "GameKey", "00000000000000000000000000000000" );
        const Char * GameAnalytics_GameSecret = CONFIG_VALUE( "GameAnalytics", "GameSecret", "0000000000000000000000000000000000000000" );

        if( MENGINE_STRCMP( GameAnalytics_GameKey, "00000000000000000000000000000000" ) == 0 )
        {
            LOGGER_ERROR( "[GameAnalytics] need setup GameKey" );

            return false;
        }

        if( MENGINE_STRCMP( GameAnalytics_GameSecret, "0000000000000000000000000000000000000000" ) == 0 )
        {
            LOGGER_ERROR( "[GameAnalytics] need setup GameSecret" );

            return false;
        }

        MENGINE_STRCPY( m_gameKey, GameAnalytics_GameKey );
        MENGINE_STRCPY( m_gameSecret, GameAnalytics_GameSecret );

        Char analyticsPath[MENGINE_MAX_PATH] = {'\0'};

        Char userPath[MENGINE_MAX_PATH] = {'\0'};
        PLATFORM_SERVICE()
            ->getUserPath( userPath );

        MENGINE_STRCAT( analyticsPath, userPath );
        MENGINE_STRCAT( analyticsPath, ".game-analytics" );

        gameanalytics::GameAnalytics::configureWritablePath( analyticsPath );

        bool GameAnalytics_InfoLog = CONFIG_VALUE( "GameAnalytics", "InfoLog", false );
        gameanalytics::GameAnalytics::setEnabledInfoLog( GameAnalytics_InfoLog );

        bool GameAnalytics_VerboseLog = CONFIG_VALUE( "GameAnalytics", "VerboseLog", false );
        gameanalytics::GameAnalytics::setEnabledVerboseLog( GameAnalytics_VerboseLog );

        const Char * EngineGITSHA1 = Helper::getEngineGITSHA1();
        gameanalytics::GameAnalytics::configureSdkGameEngineVersion( EngineGITSHA1 );

        const Char * buildVersion = Helper::getBuildVersion();
        gameanalytics::GameAnalytics::configureGameEngineVersion( buildVersion );

        LOGGER_INFO( "plugin", "plugin '%s' create system: %s"
            , this->getPluginName()
            , "AnalyticsSystem"
        );

        NOTIFICATION_ADDOBSERVERMETHOD_THIS( NOTIFICATOR_ACCOUNTS_LOAD, &GameAnalyticsPlugin::notifyAccountsLoad_, MENGINE_DOCUMENT_FACTORABLE );

        if( SERVICE_CREATE( AnalyticsSystem, MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void GameAnalyticsPlugin::notifyAccountsLoad_()
    {
        Char globalUUID[MENGINE_ACCOUNT_UUID_SIZE] = {'\0'};
        ACCOUNT_SERVICE()
            ->getGlobalUUID( globalUUID );

        gameanalytics::GameAnalytics::configureUserId( globalUUID );

        gameanalytics::GameAnalytics::initialize( m_gameKey, m_gameSecret );
    }
    //////////////////////////////////////////////////////////////////////////
    void GameAnalyticsPlugin::_finalizePlugin()
    {
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_ACCOUNTS_LOAD );

        SERVICE_FINALIZE( AnalyticsSystem );

        gameanalytics::GameAnalytics::onQuit();
    }
    //////////////////////////////////////////////////////////////////////////
    void GameAnalyticsPlugin::_destroyPlugin()
    {
        SERVICE_DESTROY( AnalyticsSystem );
    }
    //////////////////////////////////////////////////////////////////////////
}