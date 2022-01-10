#include "GameAnalyticsPlugin.h"

#include "Interface/PlatformInterface.h"
#include "Interface/OptionsServiceInterface.h"
#include "Interface/ConfigServiceInterface.h"
#include "Interface/FileServiceInterface.h"
#include "Interface/LoggerServiceInterface.h"

#include "Plugins/JSONPlugin/JSONInterface.h"

#include "GameAnalytics.h"

#include "Kernel/Logger.h"
#include "Kernel/AssertionAllocator.h"
#include "Kernel/FilePathDateTimeHelper.h"
#include "Kernel/ConfigHelper.h"
#include "Kernel/BuildMode.h"
#include "Kernel/ArrayString.h"
#include "Kernel/FilePathHelper.h"
#include "Kernel/FileStreamHelper.h"
#include "Kernel/IniHelper.h"
#include "Kernel/StaticString.h"
#include "Kernel/UID.h"

#include "Config/StdString.h"

#ifndef MENGINE_GAMEANALYTICS_FOLDER
#define MENGINE_GAMEANALYTICS_FOLDER "game-analytics"
#endif

//////////////////////////////////////////////////////////////////////////
SERVICE_EXTERN( AnalyticsSystem );
//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( GameAnalytics, Mengine::GameAnalyticsPlugin );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        static void logHandler( const char * _message, gameanalytics::EGALoggerMessageType _type )
        {
            ELoggerLevel level = LM_SILENT;

            switch( _type )
            {
            case gameanalytics::LogError:
                level = LM_ERROR;
                break;
            case gameanalytics::LogWarning:
                level = LM_WARNING;
                break;
            case gameanalytics::LogInfo:
                level = LM_INFO;
                break;
            case gameanalytics::LogDebug:
                level = LM_DEBUG;
                break;
            default:
                break;
            }

            StaticString<4096> messageln;
            messageln.assign( _message );
            messageln.append( '\n' );

            const Char * messageln_str = messageln.c_str();
            size_t messageln_len = messageln.size();

            LOGGER_SERVICE()
                ->logMessage( level, 0, LCOLOR_GREEN, messageln_str, messageln_len );
        }
        //////////////////////////////////////////////////////////////////////////
    }
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
        if( HAS_OPTION( "noanalytics" ) == true )
        {
            return false;
        }

        if( HAS_OPTION( "analytics" ) == false )
        {
            bool Engine_GameAnalyticsPluginAvailable = CONFIG_VALUE( "Engine", "GameAnalyticsPluginAvailable", true );

            if( Engine_GameAnalyticsPluginAvailable == false )
            {
                return false;
            }

            bool GameAnalytics_Enable = CONFIG_VALUE( "GameAnalytics", "Enable", true );

            if( GameAnalytics_Enable == false )
            {
                return false;
            }
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

        m_gameKey.assign( GameAnalytics_GameKey );
        m_gameSecret.assign( GameAnalytics_GameSecret );

        gameanalytics::GameAnalytics::configureCustomLogHandler( &Detail::logHandler );

        Char analyticsPath[MENGINE_MAX_PATH] = {'\0'};

        Char userPath[MENGINE_MAX_PATH] = {'\0'};
        PLATFORM_SERVICE()
            ->getUserPath( userPath );

        MENGINE_STRCAT( analyticsPath, userPath );
        MENGINE_STRCAT( analyticsPath, MENGINE_GAMEANALYTICS_FOLDER );

        gameanalytics::GameAnalytics::configureWritablePath( analyticsPath );

        bool GameAnalytics_InfoLog = CONFIG_VALUE( "GameAnalytics", "InfoLog", false );
        gameanalytics::GameAnalytics::setEnabledInfoLog( GameAnalytics_InfoLog );

        bool GameAnalytics_VerboseLog = CONFIG_VALUE( "GameAnalytics", "VerboseLog", false );
        gameanalytics::GameAnalytics::setEnabledVerboseLog( GameAnalytics_VerboseLog );

        const Char * EngineGITSHA1 = Helper::getEngineGITSHA1();
        gameanalytics::GameAnalytics::configureSdkGameEngineVersion( EngineGITSHA1 );

        const Char * buildVersion = Helper::getBuildVersion();
        gameanalytics::GameAnalytics::configureGameEngineVersion( buildVersion );

        const Char * userId = this->getUserId_();
        gameanalytics::GameAnalytics::configureUserId( userId );

        gameanalytics::GameAnalytics::setEnabledManualSessionHandling( true );

        gameanalytics::GameAnalytics::initialize( m_gameKey.c_str(), m_gameSecret.c_str() );

        LOGGER_INFO( "plugin", "plugin '%s' create system: %s"
            , this->getPluginName()
            , "AnalyticsSystem"
        );

        if( SERVICE_CREATE( AnalyticsSystem, MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void GameAnalyticsPlugin::_finalizePlugin()
    {
        SERVICE_FINALIZE( AnalyticsSystem );

        gameanalytics::GameAnalytics::onQuit();
    }
    //////////////////////////////////////////////////////////////////////////
    void GameAnalyticsPlugin::_destroyPlugin()
    {
        SERVICE_DESTROY( AnalyticsSystem );
    }
    //////////////////////////////////////////////////////////////////////////
    bool GameAnalyticsPlugin::loadUserId_()
    {
        const FileGroupInterfacePtr & fileGroupUser = FILE_SERVICE()
            ->getFileGroup( STRINGIZE_STRING_LOCAL( "user" ) );

        Char configPath[MENGINE_MAX_PATH] = {'\0'};
        MENGINE_STRCAT( configPath, MENGINE_GAMEANALYTICS_FOLDER );
        MENGINE_STRCAT( configPath, "/" );

        const Char * GameAnalytics_Config = CONFIG_VALUE( "GameAnalytics", "Config", "config.json" );

        MENGINE_STRCAT( configPath, GameAnalytics_Config );

        FilePath configPath_f = Helper::stringizeFilePath( configPath );

        if( fileGroupUser->existFile( configPath_f, false ) == false )
        {
            return false;
        }

        ConfigInterfacePtr analyticsConfig = CONFIG_SERVICE()
            ->loadConfig( fileGroupUser, configPath_f, STRINGIZE_STRING_LOCAL( "json" ), MENGINE_DOCUMENT_FACTORABLE );

        const Char * Config_UserId;
        if( analyticsConfig->hasValue( "Config", "UserId", "", &Config_UserId ) == false )
        {
            return false;
        }

        m_userId.assign( Config_UserId );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool GameAnalyticsPlugin::saveUserId_()
    {
        const FileGroupInterfacePtr & fileGroupUser = FILE_SERVICE()
            ->getFileGroup( STRINGIZE_STRING_LOCAL( "user" ) );

        Char configPath[MENGINE_MAX_PATH] = {'\0'};
        MENGINE_STRCAT( configPath, MENGINE_GAMEANALYTICS_FOLDER );
        MENGINE_STRCAT( configPath, "/" );

        const Char * GameAnalytics_Config = CONFIG_VALUE( "GameAnalytics", "Config", "config.json" );

        MENGINE_STRCAT( configPath, GameAnalytics_Config );

        FilePath configPath_f = Helper::stringizeFilePath( configPath );

        OutputStreamInterfacePtr file = Helper::openOutputStreamFile( fileGroupUser, configPath_f, false, MENGINE_DOCUMENT_FACTORABLE );

        if( file == nullptr )
        {
            return false;
        }

        jpp::object j_root = jpp::make_object();

        jpp::object j_config = jpp::make_object();
        j_config.set( "UserId", m_userId.c_str() );

        j_root.set( "Config", j_root );

        if( JSON_SERVICE()
            ->saveJSON( file, j_root ) == false )
        {
            return false;
        }

        if( fileGroupUser->closeOutputFile( file ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    const Char * GameAnalyticsPlugin::getUserId_()
    {
        if( this->loadUserId_() == true )
        {
            return m_userId.c_str();
        }

        Helper::makeUID( 20, m_userId.data() );
        m_userId.change( 20, '\0' );

        this->saveUserId_();

        return m_userId.c_str();
    }
    //////////////////////////////////////////////////////////////////////////
}