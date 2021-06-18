#include "SentryPlugin.h"

#include "Interface/PlatformInterface.h"
#include "Interface/ApplicationInterface.h"
#include "Interface/NotificatorInterface.h"
#include "Interface/NotificationServiceInterface.h"
#include "Interface/ConfigServiceInterface.h"
#include "Interface/OptionsServiceInterface.h"
#include "Interface/LoggerServiceInterface.h"

#include "Kernel/Crash.h"
#include "Kernel/Stringalized.h"
#include "Kernel/UnicodeHelper.h"
#include "Kernel/PathString.h"
#include "Kernel/Logger.h"
#include "Kernel/UID.h"
#include "Kernel/BuildMode.h"

#include "Config/GitSHA1.h"
#include "Config/BuildVersion.h"
#include "Config/StdString.h"
#include "Config/StdIO.h"

#include "sentry.h"

//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( Sentry, Mengine::SentryPlugin )
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        static void sentry_logger_func( sentry_level_t _level, const char * _format, va_list _args, void * _ud )
        {
            MENGINE_UNUSED( _ud );

            ELoggerLevel level;
            switch( _level )
            {
            case SENTRY_LEVEL_DEBUG:
                {
                    return;
                }break;
            case SENTRY_LEVEL_INFO:
                {
                    level = LM_INFO;
                }break;
            case SENTRY_LEVEL_WARNING:
                {
                    level = LM_WARNING;
                }break;
            case SENTRY_LEVEL_ERROR:
                {
                    level = LM_ERROR;
                }break;
            case SENTRY_LEVEL_FATAL:
                {
                    level = LM_FATAL;
                }break;
            default:
                return;
            }

            Char message[2048] = {'\0'};
            int32_t size_vsnprintf = MENGINE_VSNPRINTF( message, 2048, _format, _args );

            if( size_vsnprintf < 0 )
            {
                LOGGER_ERROR( "invalid string format '%s'"
                    , _format
                );

                return;
            }

            MENGINE_STRCAT( message, "\n" );

            LOGGER_SERVICE()
                ->logMessage( level, 0, LCOLOR_GREEN, message, (size_t)size_vsnprintf + 1 );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    SentryPlugin::SentryPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    SentryPlugin::~SentryPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool SentryPlugin::_unimportantPlugin() const
    {
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SentryPlugin::_initializePlugin()
    {
        if( CONFIG_VALUE( "Sentry", "Enable", true ) == false )
        {
            return true;
        }

        if( HAS_OPTION( "nosentry" ) == true )
        {
            return true;
        }

#ifdef MENGINE_DEBUG
        if( PLATFORM_SERVICE()
            ->isDebuggerPresent() == true && HAS_OPTION( "sentrydebug" ) == false )
        {
            return true;
        }
#endif

        LOGGER_MESSAGE_RELEASE( "Sentry: %s"
            , SENTRY_SDK_USER_AGENT
        );

        const Char * sentryDSN = CONFIG_VALUE( "Sentry", "DSN", "" );

        if( MENGINE_STRCMP( sentryDSN, "" ) == 0 )
        {
            LOGGER_WARNING( "Sentry don't setup DSN" );

            return true;
        }

        LOGGER_MESSAGE( "Sentry DSN: %s"
            , sentryDSN
        );

#ifdef MENGINE_PLATFORM_WINDOWS
        const Char * sentryHandler = CONFIG_VALUE( "Sentry", "Handler", "crashpad_handler.exe" );
#else
        const Char * sentryHandler = CONFIG_VALUE( "Sentry", "Handler", "crashpad_handler" );
#endif

        LOGGER_MESSAGE( "Sentry Handler: %s"
            , sentryHandler
        );

        sentry_options_t * options = sentry_options_new();

        sentry_options_set_logger( options, &Detail::sentry_logger_func, nullptr );

        Char userPath[MENGINE_MAX_PATH] = {'\0'};
        size_t userPathLen = PLATFORM_SERVICE()
            ->getUserPath( userPath );

        PathString sentryDatabasePath;
        sentryDatabasePath.append( userPath, (PathString::size_type)userPathLen );
        sentryDatabasePath.append( ".sentry-native" );
        
        LOGGER_MESSAGE( "Sentry Database: %s"
            , sentryDatabasePath.c_str()
        );

#ifdef MENGINE_PLATFORM_WINDOWS
        WChar unicode_sentryDatabasePath[MENGINE_MAX_PATH] = {L'\0'};
        Helper::utf8ToUnicode( sentryDatabasePath, unicode_sentryDatabasePath, MENGINE_MAX_PATH, nullptr );

        sentry_options_set_database_pathw( options, unicode_sentryDatabasePath );
#else
        const Char * str_sentryDatabasePath = sentryDatabasePath.c_str();
        
        sentry_options_set_database_path( options, str_sentryDatabasePath );
#endif
        
        Char currentPath[MENGINE_MAX_PATH] = {'\0'};
        size_t currentPathLen = PLATFORM_SERVICE()
            ->getCurrentPath( currentPath );
        
        PathString sentryHandlerPath;
        sentryHandlerPath.append( currentPath, (PathString::size_type)currentPathLen );
        sentryHandlerPath.append( sentryHandler );

        LOGGER_MESSAGE( "Sentry Handler: %s"
            , sentryHandlerPath.c_str()
        );
        
#ifdef MENGINE_PLATFORM_WINDOWS
        WChar unicode_sentryHandlerPath[MENGINE_MAX_PATH] = {L'\0'};
        Helper::utf8ToUnicode( sentryHandlerPath, unicode_sentryHandlerPath, MENGINE_MAX_PATH, nullptr );

        sentry_options_set_handler_pathw( options, unicode_sentryHandlerPath );
#else
        const Char * str_sentryHandlerPath = sentryHandlerPath.c_str();
        
        sentry_options_set_handler_path( options, str_sentryHandlerPath );
#endif

        sentry_options_set_dsn( options, sentryDSN ); 
        sentry_options_set_system_crash_reporter_enabled( options, 1 );
        sentry_options_set_debug( options, MENGINE_DEBUG_VALUE( 1, 0 ) );

        sentry_options_set_release( options, MENGINE_BUILD_VERSION );

        if( sentry_init( options ) != 0 )
        {
            LOGGER_ERROR( "invalid initialize sentry plugin" );

            return false;
        }

        NOTIFICATION_ADDOBSERVERMETHOD_THIS( NOTIFICATOR_BOOTSTRAPPER_CREATE_APPLICATION, &SentryPlugin::notifyCreateApplication_, MENGINE_DOCUMENT_FACTORABLE );
        NOTIFICATION_ADDOBSERVERMETHOD_THIS( NOTIFICATOR_ASSERTION, &SentryPlugin::notifyAssertion_, MENGINE_DOCUMENT_FACTORABLE );
        NOTIFICATION_ADDOBSERVERMETHOD_THIS( NOTIFICATOR_ERROR, &SentryPlugin::notifyError_, MENGINE_DOCUMENT_FACTORABLE );

        SentryLoggerCapturePtr loggerCapture = Helper::makeFactorableUnique<SentryLoggerCapture>( MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( loggerCapture );

        loggerCapture->setVerboseLevel( LM_ERROR );

        LOGGER_SERVICE()
            ->registerLogger( loggerCapture );

        m_loggerCapture = loggerCapture;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void SentryPlugin::_finalizePlugin()
    {
        if( CONFIG_VALUE( "Sentry", "Enable", true ) == false )
        {
            return;
        }

        if( HAS_OPTION( "nosentry" ) == true )
        {
            return;
        }

        if( PLATFORM_SERVICE()
            ->isDebuggerPresent() == true && HAS_OPTION( "sentrydebug" ) == false )
        {
            return;
        }

        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_BOOTSTRAPPER_CREATE_APPLICATION );
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_ASSERTION );
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_ERROR );

        LOGGER_SERVICE()
            ->unregisterLogger( m_loggerCapture );

        m_loggerCapture = nullptr;

        sentry_close();
    }
    //////////////////////////////////////////////////////////////////////////
    void SentryPlugin::notifyAssertion_( uint32_t _level, const Char * _test, const Char * _file, int32_t _line, const Char * _message )
    {
        if( _level < ASSERTION_LEVEL_FATAL )
        {
            return;
        }

        sentry_set_extra( "Assetion Test", sentry_value_new_string( _test ) );
        sentry_set_extra( "Assetion Function", sentry_value_new_string( _file ) );
        sentry_set_extra( "Assetion Line", sentry_value_new_int32( _line ) );

        sentry_value_t event = sentry_value_new_message_event( SENTRY_LEVEL_ERROR, "Assertion", _message );

        sentry_capture_event( event );
    }
    //////////////////////////////////////////////////////////////////////////
    void SentryPlugin::notifyError_( EErrorLevel _level, const Char * _file, int32_t _line, const Char * _message )
    {
        sentry_set_extra( "Error Level", sentry_value_new_int32( _level ) );
        sentry_set_extra( "Error Function", sentry_value_new_string( _file ) );
        sentry_set_extra( "Error Line", sentry_value_new_int32( _line ) );

        sentry_value_t event = sentry_value_new_message_event( SENTRY_LEVEL_ERROR, "Error", _message );

        sentry_capture_event( event );
    }
    //////////////////////////////////////////////////////////////////////////
    void SentryPlugin::notifyCreateApplication_()
    {
        sentry_value_t screen = sentry_value_new_object();
        sentry_value_set_by_key( screen, "width", sentry_value_new_int32( 1920 ) );
        sentry_value_set_by_key( screen, "height", sentry_value_new_int32( 1080 ) );
        sentry_set_extra( "screen_size", screen );

        const Char * sentryApplication = CONFIG_VALUE( "Sentry", "Application", "Mengine" );

        LOGGER_MESSAGE( "Sentry set extra [Application: %s]"
            , sentryApplication
        );

        sentry_set_extra( "Application", sentry_value_new_string( "Mengine" ) );

        Char companyName[MENGINE_PLATFORM_PROJECT_TITLE_MAXNAME] = {'\0'};
        APPLICATION_SERVICE()
            ->getCompanyName( companyName );

        LOGGER_MESSAGE( "Sentry set extra [Company: %s]"
            , companyName
        );

        sentry_set_extra( "Company", sentry_value_new_string( companyName ) );

        Char projectName[MENGINE_PLATFORM_PROJECT_TITLE_MAXNAME] = {'\0'};
        APPLICATION_SERVICE()
            ->getProjectName( projectName );

        LOGGER_MESSAGE( "Sentry set extra [Project: %s]"
            , projectName
        );

        sentry_set_extra( "Project", sentry_value_new_string( projectName ) );

#ifdef MENGINE_DEBUG
        Char userName[MENGINE_PLATFORM_USER_MAXNAME] = {'\0'};
        PLATFORM_SERVICE()
            ->getUserName( userName );

        LOGGER_MESSAGE( "Sentry set extra [User: %s]"
            , userName
        );

        sentry_set_extra( "User", sentry_value_new_string( userName ) );
#endif

        uint32_t projectVersion = APPLICATION_SERVICE()
            ->getProjectVersion();

        Char projectVersionString[32] = {'\0'};
        if( Helper::stringalized( projectVersion, projectVersionString, 31 ) == false )
        {
            LOGGER_MESSAGE( "Sentry set extra [Version: %s]"
                , "Error"
            );

            sentry_set_extra( "Version", sentry_value_new_string( "Error" ) );
        }
        else
        {
            LOGGER_MESSAGE( "Sentry set extra [Version: %s]"
                , projectVersionString
            );

            sentry_set_extra( "Version", sentry_value_new_string( projectVersionString ) );
        }

        bool debugMode = MENGINE_DEBUG_VALUE( true, false );

        LOGGER_MESSAGE( "Sentry set extra [Debug: %u]"
            , debugMode
        );

        sentry_set_extra( "Debug", sentry_value_new_bool( debugMode ) );

        bool developmentMode = Helper::isDevelopmentMode();

        LOGGER_MESSAGE( "Sentry set extra [Development: %u]"
            , developmentMode
        );

        sentry_set_extra( "Development", sentry_value_new_bool( developmentMode ) );

        bool masterMode = MENGINE_MASTER_VALUE( true, false );

        LOGGER_MESSAGE( "Sentry set extra [Master: %u]"
            , masterMode
        );

        sentry_set_extra( "Master", sentry_value_new_bool( masterMode ) );

        bool publishMode = MENGINE_PUBLISH_VALUE( true, false );

        LOGGER_MESSAGE( "Sentry set extra [Publish: %u]"
            , publishMode
        );

        sentry_set_extra( "Publish", sentry_value_new_bool( publishMode ) );

#ifdef MENGINE_ENGINE_GIT_SHA1
        const Char * ENGINE_GIT_SHA1 = MENGINE_ENGINE_GIT_SHA1;

        LOGGER_MESSAGE( "Sentry set extra [Engine Commit: %s]"
            , ENGINE_GIT_SHA1
        );

        sentry_set_extra( "Engine Commit", sentry_value_new_string( ENGINE_GIT_SHA1 ) );
#endif

#ifdef MENGINE_BUILD_TIMESTAMP
        const Char * BUILD_TIMESTAMP = MENGINE_BUILD_TIMESTAMP;

        LOGGER_MESSAGE( "Sentry set extra [Build Timestamp: %s]"
            , BUILD_TIMESTAMP
        );

        sentry_set_extra( "Build Timestamp", sentry_value_new_string( BUILD_TIMESTAMP ) );
#endif

        const Char * Info_ResourceCommit = CONFIG_VALUE( "Info", "ResourceCommit", "0000000000000000000000000000000000000000" );

        LOGGER_MESSAGE( "Sentry set extra [Resource Commit: %s]"
            , Info_ResourceCommit
        );

        sentry_set_extra( "Resource Commit", sentry_value_new_string( Info_ResourceCommit ) );

        if( HAS_OPTION( "sentrycrash" ) == true )
        {
            LOGGER_MESSAGE_RELEASE( "!!!test sentry crash!!!" );

            Char message_uid[21] = {'\0'};
            Helper::makeUID( 20, message_uid );
            message_uid[20] = '\0';

            LOGGER_MESSAGE_RELEASE( "uid: %s", message_uid );
            LOGGER_MESSAGE_RELEASE( "!!!test sentry crash!!!" );

            sentry_value_t event = sentry_value_new_message_event( SENTRY_LEVEL_ERROR, "Test", "sentrycrash" );

            sentry_capture_event( event );

            Helper::crash( "sentrycrash" );
        }
    }
    //////////////////////////////////////////////////////////////////////////
}
