#include "SentryPlugin.h"

#include "Interface/PlatformInterface.h"
#include "Interface/ApplicationInterface.h"
#include "Interface/NotificatorInterface.h"
#include "Interface/NotificationServiceInterface.h"
#include "Interface/ConfigServiceInterface.h"
#include "Interface/OptionsServiceInterface.h"

#include "Kernel/Stringalized.h"
#include "Kernel/PathString.h"
#include "Kernel/Logger.h"

#include "Config/GitSHA1.h"
#include "Config/StdString.h"
#include "Config/StdIO.h"

//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( Sentry, Mengine::SentryPlugin )
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    SentryPlugin::SentryPlugin()
        : m_options( nullptr )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    SentryPlugin::~SentryPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool SentryPlugin::_initializePlugin()
    {
        if( CONFIG_VALUE( "Sentry", "Enable", true ) == false )
        {
            return true;
        }

        if( PLATFORM_SERVICE()
            ->isDebuggerPresent() == true && HAS_OPTION( "sentrydebug" ) == false )
        {
            return true;
        }

        LOGGER_MESSAGE( "Sentry: %s"
            , SENTRY_SDK_USER_AGENT
        );

        const Char * sentryDSN = CONFIG_VALUE( "Sentry", "DSN", "" );

        if( MENGINE_STRCMP( sentryDSN, "" ) == 0 )
        {
            LOGGER_WARNING( "Sentry don't setup DSN" );

            return true;
        }

        const Char * sentryHandler = CONFIG_VALUE( "Sentry", "Handler", "crashpad_handler.exe" );

        LOGGER_MESSAGE( "Sentry Handler: %s"
            , sentryHandler
        );

        sentry_options_t * options = sentry_options_new();

        Char userPath[MENGINE_MAX_PATH] = { 0 };
        size_t userPathLen = PLATFORM_SERVICE()
            ->getUserPath( userPath );

        PathString sentryDatabasePath;
        sentryDatabasePath.append( userPath, (PathString::size_type)userPathLen );
        sentryDatabasePath.append( ".sentry-native" );

        sentry_options_set_database_path( options, sentryDatabasePath.c_str() );
        sentry_options_set_dsn( options, sentryDSN );
        sentry_options_set_handler_path( options, sentryHandler );
        sentry_options_set_system_crash_reporter_enabled( options, 1 );
        sentry_options_set_debug( options, 1 );
        sentry_init( options );

        m_options = options;

        const Char * sentryApplication = CONFIG_VALUE( "Sentry", "Application", "Mengine" );

        LOGGER_MESSAGE( "Sentry Application: %s"
            , sentryApplication
        );

        sentry_set_extra( "Application", sentry_value_new_string( "Mengine" ) );

        NOTIFICATION_ADDOBSERVERMETHOD( NOTIFICATOR_BOOTSTRAPPER_CREATE_APPLICATION, this, &SentryPlugin::notifyCreateApplication_, MENGINE_DOCUMENT_FACTORABLE );
        NOTIFICATION_ADDOBSERVERMETHOD( NOTIFICATOR_ASSERTION, this, &SentryPlugin::notifyAssertion_, MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void SentryPlugin::_finalizePlugin()
    {
        if( PLATFORM_SERVICE()
            ->isDebuggerPresent() == true && HAS_OPTION( "sentrydebug" ) == false )
        {
            return;
        }

        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_BOOTSTRAPPER_CREATE_APPLICATION );
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_ASSERTION );

        sentry_shutdown();
    }
    //////////////////////////////////////////////////////////////////////////
    void SentryPlugin::_destroyPlugin()
    {
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
    void SentryPlugin::notifyCreateApplication_()
    {
        Char companyName[MENGINE_PLATFORM_PROJECT_TITLE_MAXNAME] = {0};
        APPLICATION_SERVICE()
            ->getCompanyName( companyName );

        sentry_set_extra( "Company", sentry_value_new_string( companyName ) );

        Char projectName[MENGINE_PLATFORM_PROJECT_TITLE_MAXNAME] = {0};
        APPLICATION_SERVICE()
            ->getProjectName( projectName );

        sentry_set_extra( "Project", sentry_value_new_string( projectName ) );

        Char userName[MENGINE_PLATFORM_USER_MAXNAME] = {0};
        PLATFORM_SERVICE()
            ->getUserName( userName );

        sentry_set_extra( "User", sentry_value_new_string( userName ) );

        uint32_t projectVersion = APPLICATION_SERVICE()
            ->getProjectVersion();

        Char projectVersionString[32] = {0};
        if( Helper::stringalized( projectVersion, projectVersionString, 32 ) == false )
        {
            sentry_set_extra( "Version", sentry_value_new_string( "Error" ) );
        }
        else
        {
            sentry_set_extra( "Version", sentry_value_new_string( projectVersionString ) );
        }

        sentry_set_extra( "Debug", sentry_value_new_bool( MENGINE_DEBUG_ATTRIBUTE( true, false ) ) );

        bool developmentMode = HAS_OPTION( "dev" );

        sentry_set_extra( "Development", sentry_value_new_bool( developmentMode == true ? 1 : 0 ) );

        bool masterMode = MENGINE_MASTER_ATTRIBUTE( true, false );

        sentry_set_extra( "Master", sentry_value_new_bool( masterMode ) );

#ifdef MENGINE_GIT_SHA1
        sentry_set_extra( "Commit", sentry_value_new_string( MENGINE_GIT_SHA1 ) );
#endif

        Char releaseString[256];
        MENGINE_SNPRINTF( releaseString, 256, "%s@%u", projectName, projectVersion );

        sentry_options_set_release( m_options, releaseString );

        if( HAS_OPTION( "sentrycrash" ) == true )
        {
            LOGGER_MESSAGE_RELEASE( "!!!test sentry crash!!!" );

            sentry_value_t event = sentry_value_new_message_event( SENTRY_LEVEL_ERROR, "Test", "sentrycrash" );

            sentry_capture_event( event );

            volatile uint32_t * p = nullptr;
            *p = 0xBADC0DE;
        }
    }
}
