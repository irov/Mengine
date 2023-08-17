#include "Win32SentryService.h"

#include "Interface/PlatformServiceInterface.h"
#include "Interface/ApplicationInterface.h"
#include "Interface/LoggerServiceInterface.h"
#include "Interface/DateTimeSystemInterface.h"

#if defined(MENGINE_WINDOWS_DEBUG)
#   include "Environment/Windows/Win32Helper.h"
#endif

#include "Kernel/ConfigHelper.h"
#include "Kernel/Crash.h"
#include "Kernel/Stringalized.h"
#include "Kernel/UnicodeHelper.h"
#include "Kernel/PathString.h"
#include "Kernel/Logger.h"
#include "Kernel/LoggerHelper.h"
#include "Kernel/UID.h"
#include "Kernel/BuildMode.h"
#include "Kernel/OptionHelper.h"
#include "Kernel/NotificationHelper.h"

#include "Config/StdString.h"
#include "Config/StdIO.h"

#include "sentry.h"

//////////////////////////////////////////////////////////////////////////
#ifndef MENGINE_SENTRY_DATABASE_PATH
#define MENGINE_SENTRY_DATABASE_PATH "sentry-native"
#endif
//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( Win32SentryService, Mengine::Win32SentryService );
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
                    bool OPTION_sentrytrace = HAS_OPTION( "sentrytrace" );

                    if( OPTION_sentrytrace == true )
                    {
                        level = LM_DEBUG;
                    }
                    else
                    {
                        return;
                    }
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

            LoggerOperator( "sentry", level, 0, LCOLOR_GREEN, nullptr, 0, LFLAG_SHORT ).logMessageArgs( _format, _args );
        }
        //////////////////////////////////////////////////////////////////////////
#if defined(MENGINE_WINDOWS_DEBUG)
        //////////////////////////////////////////////////////////////////////////
        sentry_value_t sentry_crash_function( const sentry_ucontext_t * _uctx, sentry_value_t _event, void * _closure )
        {
            MENGINE_UNUSED( _uctx );
            MENGINE_UNUSED( _closure );

            Helper::Win32Toast( L"Engine caught crash [Sentry]" );

            return _event;
        }
        //////////////////////////////////////////////////////////////////////////
#endif
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
    Win32SentryService::Win32SentryService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Win32SentryService::~Win32SentryService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32SentryService::_initializeService()
    {
        bool OPTION_sentrydebug = HAS_OPTION( "sentrydebug" );

#if defined(MENGINE_DEBUG)
        if( PLATFORM_SERVICE()
            ->isDebuggerPresent() == true && OPTION_sentrydebug == false )
        {
            return true;
        }
#endif

        LOGGER_MESSAGE( "Sentry version: %s name: %s agent: %s"
            , sentry_sdk_version()
            , sentry_sdk_name()
            , sentry_sdk_user_agent()
        );

        const Char * Win32SentryPlugin_DSN = CONFIG_VALUE( "Win32SentryPlugin", "DSN", "" );

        if( MENGINE_STRCMP( Win32SentryPlugin_DSN, "" ) == 0 )
        {
            LOGGER_WARNING( "Sentry don't setup DSN" );

            return true;
        }

        LOGGER_INFO_PROTECTED( "sentry", "Sentry DSN: %s"
            , Win32SentryPlugin_DSN
        );

        sentry_options_t * options = sentry_options_new();

#if defined(MENGINE_DEBUG)
        sentry_options_set_debug( options, 1 );
#else
        if( OPTION_sentrydebug == true )
        {
            sentry_options_set_debug( options, 1 );
        }
        else
        {
            sentry_options_set_debug( options, 0 );
        }
#endif

        sentry_options_set_logger( options, &Detail::sentry_logger_func, nullptr );

        Char userPath[MENGINE_MAX_PATH] = {'\0'};
        size_t userPathLen = PLATFORM_SERVICE()
            ->getUserPath( userPath );

        PathString sentryDatabasePath;
        sentryDatabasePath.append( userPath, (PathString::size_type)userPathLen );
        sentryDatabasePath.append( MENGINE_SENTRY_DATABASE_PATH );

        LOGGER_INFO_PROTECTED( "sentry", "Sentry database: %s"
            , sentryDatabasePath.c_str()
        );

#if defined(MENGINE_PLATFORM_WINDOWS)
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

#if defined(MENGINE_PLATFORM_WINDOWS)
        const Char * Win32SentryPlugin_Handler = CONFIG_VALUE( "Win32SentryPlugin", "Handler", "crashpad_handler.exe" );
#else
        const Char * Win32SentryPlugin_Handler = CONFIG_VALUE( "Win32SentryPlugin", "Handler", "crashpad_handler" );
#endif

        LOGGER_INFO_PROTECTED( "sentry", "Sentry handler: %s"
            , Win32SentryPlugin_Handler
        );

        sentryHandlerPath.append( Win32SentryPlugin_Handler );

#if defined(MENGINE_PLATFORM_WINDOWS)
        WChar unicode_sentryHandlerPath[MENGINE_MAX_PATH] = {L'\0'};
        Helper::utf8ToUnicode( sentryHandlerPath, unicode_sentryHandlerPath, MENGINE_MAX_PATH, nullptr );

        sentry_options_set_handler_pathw( options, unicode_sentryHandlerPath );
#else
        const Char * str_sentryHandlerPath = sentryHandlerPath.c_str();

        sentry_options_set_handler_path( options, str_sentryHandlerPath );
#endif

        sentry_options_set_dsn( options, Win32SentryPlugin_DSN );
        sentry_options_set_system_crash_reporter_enabled( options, 1 );

        const Char * BUILD_VERSION = Helper::getBuildVersion();
        sentry_options_set_release( options, BUILD_VERSION );

        const Char * BUILD_NUMBER_STRING = Helper::getBuildNumberString();
        sentry_options_set_dist( options, BUILD_NUMBER_STRING );

        Char environment[128] = {'\0'};

#if defined(MENGINE_BUILD_PUBLISH)
        MENGINE_STRCAT( environment, "publish_" );
#endif

#if defined(MENGINE_MASTER_RELEASE)
        MENGINE_STRCAT( environment, "master_" );
#endif

#if defined(MENGINE_DEBUG)
        MENGINE_STRCAT( environment, "debug" );
#else
        MENGINE_STRCAT( environment, "release" );
#endif
        
        sentry_options_set_environment( options, environment );

#if defined(MENGINE_WINDOWS_DEBUG)
        sentry_options_set_on_crash( options, &Detail::sentry_crash_function, nullptr );
#endif

        if( sentry_init( options ) != 0 )
        {
            LOGGER_ERROR( "invalid initialize sentry plugin" );

            return false;
        }

        if( sentry_get_crashed_last_run() == 1 )
        {
            sentry_clear_crashed_last_run();

#if defined(MENGINE_WINDOWS_DEBUG)
            Helper::Win32Toast( L"application has crashed on the last run" );
#endif
        }

        NOTIFICATION_ADDOBSERVERMETHOD_THIS( NOTIFICATOR_BOOTSTRAPPER_CREATE_APPLICATION, &Win32SentryService::notifyBootstrapperCreateApplication_, MENGINE_DOCUMENT_FACTORABLE );
        NOTIFICATION_ADDOBSERVERMETHOD_THIS( NOTIFICATOR_ASSERTION, &Win32SentryService::notifyAssertion_, MENGINE_DOCUMENT_FACTORABLE );
        NOTIFICATION_ADDOBSERVERMETHOD_THIS( NOTIFICATOR_ERROR, &Win32SentryService::notifyError_, MENGINE_DOCUMENT_FACTORABLE );

        bool Win32SentryPlugin_LoggerCapture = CONFIG_VALUE( "Win32SentryPlugin", "LoggerCapture", MENGINE_MASTER_RELEASE_VALUE( true, false ) );

        if( OPTION_sentrydebug == true || Win32SentryPlugin_LoggerCapture == true )
        {
            Win32SentryLoggerCapturePtr loggerCapture = Helper::makeFactorableUnique<Win32SentryLoggerCapture>( MENGINE_DOCUMENT_FACTORABLE );

            MENGINE_ASSERTION_MEMORY_PANIC( loggerCapture );

            uint32_t loggerFilter = ~0u & ~(LFILTER_PROTECTED);
            loggerCapture->setVerboseFilter( loggerFilter );
            loggerCapture->setVerboseLevel( LM_ERROR );
            loggerCapture->setWriteHistory( true );

            LOGGER_SERVICE()
                ->registerLogger( loggerCapture );

            m_loggerCapture = loggerCapture;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32SentryService::_finalizeService()
    {
#if defined(MENGINE_DEBUG)
        if( PLATFORM_SERVICE()
            ->isDebuggerPresent() == true && HAS_OPTION( "sentrydebug" ) == false )
        {
            return;
        }
#endif

        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_BOOTSTRAPPER_CREATE_APPLICATION );
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_ASSERTION );
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_ERROR );
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_ENGINE_STOP );

        if( m_loggerCapture != nullptr )
        {
            LOGGER_SERVICE()
                ->unregisterLogger( m_loggerCapture );

            m_loggerCapture = nullptr;
        }

        sentry_close();
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32SentryService::notifyAssertion_( const Char * _category, EAssertionLevel _level, const Char * _test, const Char * _file, int32_t _line, const Char * _message )
    {
        MENGINE_UNUSED( _category );

        if( _level < ASSERTION_LEVEL_FATAL )
        {
            return;
        }

        sentry_set_extra( "Assetion Test", sentry_value_new_string( _test ) );
        sentry_set_extra( "Assetion Function", sentry_value_new_string( _file ) );
        sentry_set_extra( "Assetion Line", sentry_value_new_int32( _line ) );

        sentry_value_t event = sentry_value_new_message_event( SENTRY_LEVEL_WARNING, "Assertion", _message );

        sentry_capture_event( event );
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32SentryService::notifyError_( const Char * _category, EErrorLevel _level, const Char * _file, int32_t _line, const Char * _message )
    {
        MENGINE_UNUSED( _category );

        sentry_set_extra( "Error Level", sentry_value_new_int32( _level ) );
        sentry_set_extra( "Error Function", sentry_value_new_string( _file ) );
        sentry_set_extra( "Error Line", sentry_value_new_int32( _line ) );

        sentry_value_t event = sentry_value_new_message_event( SENTRY_LEVEL_ERROR, "Error", _message );

        sentry_capture_event( event );
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32SentryService::notifyBootstrapperCreateApplication_()
    {
        const Char * Win32SentryPlugin_Application = CONFIG_VALUE( "Win32SentryPlugin", "Application", "Mengine" );

        LOGGER_INFO_PROTECTED( "sentry", "Sentry set extra [Application: %s]"
            , Win32SentryPlugin_Application
        );

        sentry_set_extra( "Application", sentry_value_new_string( Win32SentryPlugin_Application ) );

        Char companyName[MENGINE_APPLICATION_COMPANY_MAXNAME] = {'\0'};
        APPLICATION_SERVICE()
            ->getCompanyName( companyName );

        LOGGER_INFO_PROTECTED( "sentry", "Sentry set extra [Company: %s]"
            , companyName
        );

        sentry_set_extra( "Company", sentry_value_new_string( companyName ) );

        Char projectName[MENGINE_APPLICATION_PROJECT_MAXNAME] = {'\0'};
        APPLICATION_SERVICE()
            ->getProjectName( projectName );

        LOGGER_INFO_PROTECTED( "sentry", "Sentry set extra [Project: %s]"
            , projectName
        );

        sentry_set_extra( "Project", sentry_value_new_string( projectName ) );

        LOGGER_INFO_PROTECTED( "sentry", "Sentry set tag [project: %s]"
            , projectName
        );

        sentry_set_tag( "project", projectName );

#if defined(MENGINE_DEBUG)
        Char userName[MENGINE_PLATFORM_USER_MAXNAME] = {'\0'};
        PLATFORM_SERVICE()
            ->getUserName( userName );

        LOGGER_INFO_PROTECTED( "sentry", "Sentry set extra [User: %s]"
            , userName
        );

        sentry_set_extra( "User", sentry_value_new_string( userName ) );
#endif

        uint32_t projectVersion = APPLICATION_SERVICE()
            ->getProjectVersion();

        Char projectVersionString[32] = {'\0'};
        Helper::stringalized( projectVersion, projectVersionString, 31 );
        
        LOGGER_INFO_PROTECTED( "sentry", "Sentry set extra [Version: %s]"
            , projectVersionString
        );

        sentry_set_extra( "Version", sentry_value_new_string( projectVersionString ) );

        bool debugMode = Helper::isDebugMode();

        LOGGER_INFO_PROTECTED( "sentry", "Sentry set extra [Debug: %u]"
            , debugMode
        );

        sentry_set_extra( "Debug", sentry_value_new_bool( debugMode ) );

        bool developmentMode = Helper::isDevelopmentMode();

        LOGGER_INFO_PROTECTED( "sentry", "Sentry set extra [Development: %u]"
            , developmentMode
        );

        sentry_set_extra( "Development", sentry_value_new_bool( developmentMode ) );

        bool masterMode = Helper::isMasterRelease();

        LOGGER_INFO_PROTECTED( "sentry", "Sentry set extra [Master: %u]"
            , masterMode
        );

        sentry_set_extra( "Master", sentry_value_new_bool( masterMode ) );

        bool publishMode = Helper::isBuildPublish();

        LOGGER_INFO_PROTECTED( "sentry", "Sentry set extra [Publish: %u]"
            , publishMode
        );

        sentry_set_extra( "Publish", sentry_value_new_bool( publishMode ) );

        const Char * ENGINE_GIT_SHA1 = Helper::getEngineGITSHA1();

        LOGGER_INFO_PROTECTED( "sentry", "Sentry set extra [Engine Commit: %s]"
            , ENGINE_GIT_SHA1
        );

        sentry_set_extra( "Engine Commit", sentry_value_new_string( ENGINE_GIT_SHA1 ) );

        const Char * BUILD_DATE = Helper::getBuildDate();

        LOGGER_INFO_PROTECTED( "sentry", "Sentry set extra [Build Date: %s]"
            , BUILD_DATE
        );

        sentry_set_extra( "Build Date", sentry_value_new_string( BUILD_DATE ) );

        const Char * BUILD_SOLUTION_NAME = Helper::getBuildSolutionName();

        LOGGER_INFO_PROTECTED( "sentry", "Sentry set extra [Build Solution Name: %s]"
            , BUILD_SOLUTION_NAME
        );

        sentry_set_extra( "Build Solution Name", sentry_value_new_string( BUILD_SOLUTION_NAME ) );

        const Char * BUILD_USERNAME = Helper::getBuildUsername();

        LOGGER_INFO_PROTECTED( "sentry", "Sentry set extra [Build Username: %s]"
            , BUILD_USERNAME
        );

        sentry_set_extra( "Build Username", sentry_value_new_string( BUILD_USERNAME ) );

        const Char * contentCommit = Helper::getContentCommit();

        LOGGER_INFO_PROTECTED( "sentry", "Sentry set extra [Content Commit: %s]"
            , contentCommit
        );

        sentry_set_extra( "Content Commit", sentry_value_new_string( contentCommit ) );

        PlatformDateTime dateTime;
        DATETIME_SYSTEM()
            ->getLocalDateTime( &dateTime );

        Char INIT_DATE[256] = {'\0'};
        Helper::makeLoggerFullDate( dateTime, "%04u.%02u.%02u %02u:%02u:%02u:%04u", INIT_DATE, 0, 256 );

        LOGGER_INFO_PROTECTED( "sentry", "Sentry set extra [Init Date: %s]"
            , INIT_DATE
        );

        sentry_set_extra( "Init Date", sentry_value_new_string( INIT_DATE ) );

        LOGGER_INFO_PROTECTED( "sentry", "Sentry set extra [Engine Stop: %d]"
            , false
        );
    }
    //////////////////////////////////////////////////////////////////////////
}