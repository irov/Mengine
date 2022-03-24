#include "AppleSentryService.h"

#include "Interface/PlatformInterface.h"
#include "Interface/ApplicationInterface.h"
#include "Interface/NotificatorInterface.h"
#include "Interface/NotificationServiceInterface.h"
#include "Interface/LoggerServiceInterface.h"

extern "C" {
    #include "AppleSentryNative.h"
}

#include "Kernel/ConfigHelper.h"
#include "Kernel/Crash.h"
#include "Kernel/Stringalized.h"
#include "Kernel/UnicodeHelper.h"
#include "Kernel/PathString.h"
#include "Kernel/Logger.h"
#include "Kernel/UID.h"
#include "Kernel/BuildMode.h"
#include "Kernel/OptionHelper.h"

#include "Config/GitSHA1.h"
#include "Config/BuildVersion.h"
#include "Config/StdString.h"
#include "Config/StdIO.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( SentryService, Mengine::AppleSentryService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
	//////////////////////////////////////////////////////////////////////////
    AppleSentryService::AppleSentryService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AppleSentryService::~AppleSentryService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleSentryService::_initializeService()
    {
        const Char * Sentry_DSN = CONFIG_VALUE( "Sentry", "DSN", "" );

        if( MENGINE_STRCMP( Sentry_DSN, "" ) == 0 )
        {
            LOGGER_WARNING( "Sentry don't setup DSN" );

            return true;
        }

        LOGGER_MESSAGE( "Sentry DSN: %s"
            , Sentry_DSN
        );
        
        bool debugMode = Helper::isDebugMode();
        const Char * BUILD_VERSION = Helper::getBuildVersion();
        
        if( appleSentryInitialize( Sentry_DSN, debugMode, BUILD_VERSION ) != 0 )
        {
            return false;
        }

        NOTIFICATION_ADDOBSERVERMETHOD_THIS( NOTIFICATOR_BOOTSTRAPPER_CREATE_APPLICATION, &AppleSentryService::notifyCreateApplication_, MENGINE_DOCUMENT_FACTORABLE );
 	    NOTIFICATION_ADDOBSERVERMETHOD_THIS( NOTIFICATOR_ASSERTION, &AppleSentryService::notifyAssertion_, MENGINE_DOCUMENT_FACTORABLE );
        NOTIFICATION_ADDOBSERVERMETHOD_THIS( NOTIFICATOR_ERROR, &AppleSentryService::notifyError_, MENGINE_DOCUMENT_FACTORABLE );

        AppleSentryLoggerCapturePtr loggerCapture = Helper::makeFactorableUnique<AppleSentryLoggerCapture>( MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( loggerCapture );

        loggerCapture->setVerboseLevel( LM_ERROR );

        uint32_t loggerFilter = ~0u & ~(LFILTER_PROTECTED);

        loggerCapture->setVerboseFilter( loggerFilter );

        loggerCapture->setWriteHistory( true );

        LOGGER_SERVICE()
            ->registerLogger( loggerCapture );

        m_loggerCapture = loggerCapture;
        
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleSentryService::_finalizeService()
    {
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_BOOTSTRAPPER_CREATE_APPLICATION );
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_ASSERTION );
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_ERROR );
        
        LOGGER_SERVICE()
            ->unregisterLogger( m_loggerCapture );

        m_loggerCapture = nullptr;
        
        appleSentryFinalize();
    }    
    //////////////////////////////////////////////////////////////////////////
    void AppleSentryService::notifyCreateApplication_()
    {
        const Char * Sentry_Application = CONFIG_VALUE( "Sentry", "Application", "Mengine" );

        LOGGER_MESSAGE( "Sentry set extra [Application: %s]"
            , Sentry_Application
        );

        appleSentrySetExtraString( "Application", Sentry_Application );

        Char companyName[MENGINE_APPLICATION_COMPANY_MAXNAME] = {'\0'};
        APPLICATION_SERVICE()
            ->getCompanyName( companyName );

        LOGGER_MESSAGE( "Sentry set extra [Company: %s]"
            , companyName
        );

        appleSentrySetExtraString( "Company", companyName );

        Char projectName[MENGINE_PLATFORM_PROJECT_TITLE_MAXNAME] = {'\0'};
        APPLICATION_SERVICE()
            ->getProjectName( projectName );

        LOGGER_MESSAGE( "Sentry set extra [Project: %s]"
            , projectName
        );

        appleSentrySetExtraString( "Project", projectName );

#ifdef MENGINE_DEBUG
        Char userName[MENGINE_PLATFORM_USER_MAXNAME] = {'\0'};
        PLATFORM_SERVICE()
            ->getUserName( userName );

        LOGGER_MESSAGE( "Sentry set extra [User: %s]"
            , userName
        );

        appleSentrySetExtraString( "User", userName );
#endif

        uint32_t projectVersion = APPLICATION_SERVICE()
            ->getProjectVersion();

        Char projectVersionString[32] = {'\0'};
        if( Helper::stringalized( projectVersion, projectVersionString, 31 ) == false )
        {
            LOGGER_MESSAGE( "Sentry set extra [Version: %s]"
                , "Error"
            );

            appleSentrySetExtraString( "Version", "Error" );
        }
        else
        {
            LOGGER_MESSAGE( "Sentry set extra [Version: %s]"
                , projectVersionString
            );

            appleSentrySetExtraString( "Version", projectVersionString );
        }

        bool debugMode = Helper::isDebugMode();

        LOGGER_MESSAGE( "Sentry set extra [Debug: %u]"
            , debugMode
        );

        appleSentrySetExtraBoolean( "Debug", debugMode );

        bool developmentMode = Helper::isDevelopmentMode();

        LOGGER_MESSAGE( "Sentry set extra [Development: %u]"
            , developmentMode
        );

        appleSentrySetExtraBoolean( "Development", developmentMode );

        bool masterMode = Helper::isBuildMaster();

        LOGGER_MESSAGE( "Sentry set extra [Master: %u]"
            , masterMode
        );

        appleSentrySetExtraBoolean( "Master", masterMode );

        bool publishMode = Helper::isBuildPublish();

        LOGGER_MESSAGE( "Sentry set extra [Publish: %u]"
            , publishMode
        );

        appleSentrySetExtraBoolean( "Publish", publishMode );

        const Char * ENGINE_GIT_SHA1 = Helper::getEngineGITSHA1();

        LOGGER_MESSAGE( "Sentry set extra [Engine Commit: %s]"
            , ENGINE_GIT_SHA1
        );

        appleSentrySetExtraString( "Engine Commit", ENGINE_GIT_SHA1 );

        const Char * BUILD_TIMESTAMP = Helper::getBuildTimestamp();

        LOGGER_MESSAGE( "Sentry set extra [Build Timestamp: %s]"
            , BUILD_TIMESTAMP
        );

        appleSentrySetExtraString( "Build Timestamp", BUILD_TIMESTAMP );

        const Char * BUILD_USERNAME = Helper::getBuildUsername();

        LOGGER_MESSAGE( "Sentry set extra [Build Username: %s]"
            , BUILD_USERNAME
        );

        appleSentrySetExtraString( "Build Username", BUILD_USERNAME );

        const Char * contentCommit = Helper::getContentCommit();

        LOGGER_MESSAGE( "Sentry set extra [Content Commit: %s]"
            , contentCommit
        );

        appleSentrySetExtraString( "Content Commit", contentCommit );

        if( HAS_OPTION( "sentrycrash" ) == true )
        {
            LOGGER_MESSAGE_RELEASE( "!!!test sentry crash!!!" );

            Char message_uid[21] = {'\0'};
            Helper::makeUID( 20, message_uid );
            message_uid[20] = '\0';

            LOGGER_MESSAGE_RELEASE( "uid: %s", message_uid );
            LOGGER_MESSAGE_RELEASE( "!!!test sentry crash!!!" );

            appleSentryCapture( "Mengine test sentry crash", 0 );

            Helper::crash( "sentrycrash" );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleSentryService::notifyAssertion_( EAssertionLevel _level, const Char * _test, const Char * _file, int32_t _line, const Char * _message )
    {
        if( _level < ASSERTION_LEVEL_FATAL )
        {
            return;
        }

        appleSentrySetExtraString( "Assetion Test", _test );
        appleSentrySetExtraString( "Assetion Function", _file );
        appleSentrySetExtraInteger( "Assetion Line", _line );

        appleSentryCapture( _message, 0 );
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleSentryService::notifyError_( EErrorLevel _level, const Char * _file, int32_t _line, const Char * _message )
    {
        appleSentrySetExtraInteger( "Error Level", _level );
        appleSentrySetExtraString( "Error Function", _file );
        appleSentrySetExtraInteger( "Error Line", _line );

        appleSentryCapture( _message, 0 );
    }
    //////////////////////////////////////////////////////////////////////////
}
