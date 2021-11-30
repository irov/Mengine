#include "SentryMacOSPlugin.h"

#include "Interface/PlatformInterface.h"
#include "Interface/ApplicationInterface.h"
#include "Interface/NotificatorInterface.h"
#include "Interface/NotificationServiceInterface.h"
#include "Interface/OptionsServiceInterface.h"
#include "Interface/LoggerServiceInterface.h"

#include "Kernel/ConfigHelper.h"
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

//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( SentryMacOS, Mengine::SentryMacOSPlugin )
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    SentryMacOSPlugin::SentryMacOSPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    SentryMacOSPlugin::~SentryMacOSPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool SentryMacOSPlugin::_unimportantPlugin() const
    {
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SentryMacOSPlugin::_availablePlugin() const
    {
        if( HAS_OPTION( "nosentry" ) == true )
        {
            return false;
        }

        if( HAS_OPTION( "sentry" ) == false )
        {
            if( CONFIG_VALUE( "Sentry", "Enable", true ) == false )
            {
                return false;
            }
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SentryMacOSPlugin::_initializePlugin()
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

        NOTIFICATION_ADDOBSERVERMETHOD_THIS( NOTIFICATOR_BOOTSTRAPPER_CREATE_APPLICATION, &SentryPlugin::notifyCreateApplication_, MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void SentryMacOSPlugin::_finalizePlugin()
    {
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_BOOTSTRAPPER_CREATE_APPLICATION );

    }
    //////////////////////////////////////////////////////////////////////////
    void SentryMacOSPlugin::notifyCreateApplication_()
    {
        const Char * Sentry_Application = CONFIG_VALUE( "Sentry", "Application", "Mengine" );

        LOGGER_MESSAGE( "Sentry set extra [Application: %s]"
            , Sentry_Application
        );

        //sentry_set_extra( "Application", sentry_value_new_string( Sentry_Application ) );

        Char companyName[MENGINE_APPLICATION_COMPANY_MAXNAME] = {'\0'};
        APPLICATION_SERVICE()
            ->getCompanyName( companyName );

        LOGGER_MESSAGE( "Sentry set extra [Company: %s]"
            , companyName
        );

        //sentry_set_extra( "Company", sentry_value_new_string( companyName ) );

        Char projectName[MENGINE_PLATFORM_PROJECT_TITLE_MAXNAME] = {'\0'};
        APPLICATION_SERVICE()
            ->getProjectName( projectName );

        LOGGER_MESSAGE( "Sentry set extra [Project: %s]"
            , projectName
        );

        //sentry_set_extra( "Project", sentry_value_new_string( projectName ) );

#ifdef MENGINE_DEBUG
        Char userName[MENGINE_PLATFORM_USER_MAXNAME] = {'\0'};
        PLATFORM_SERVICE()
            ->getUserName( userName );

        LOGGER_MESSAGE( "Sentry set extra [User: %s]"
            , userName
        );

        //sentry_set_extra( "User", sentry_value_new_string( userName ) );
#endif

        uint32_t projectVersion = APPLICATION_SERVICE()
            ->getProjectVersion();

        Char projectVersionString[32] = {'\0'};
        if( Helper::stringalized( projectVersion, projectVersionString, 31 ) == false )
        {
            LOGGER_MESSAGE( "Sentry set extra [Version: %s]"
                , "Error"
            );

            //sentry_set_extra( "Version", sentry_value_new_string( "Error" ) );
        }
        else
        {
            LOGGER_MESSAGE( "Sentry set extra [Version: %s]"
                , projectVersionString
            );

            //sentry_set_extra( "Version", sentry_value_new_string( projectVersionString ) );
        }

        bool debugMode = Helper::isDebugMode();

        LOGGER_MESSAGE( "Sentry set extra [Debug: %u]"
            , debugMode
        );

        //sentry_set_extra( "Debug", sentry_value_new_bool( debugMode ) );

        bool developmentMode = Helper::isDevelopmentMode();

        LOGGER_MESSAGE( "Sentry set extra [Development: %u]"
            , developmentMode
        );

        //sentry_set_extra( "Development", sentry_value_new_bool( developmentMode ) );

        bool masterMode = Helper::isBuildMaster();

        LOGGER_MESSAGE( "Sentry set extra [Master: %u]"
            , masterMode
        );

        //sentry_set_extra( "Master", sentry_value_new_bool( masterMode ) );

        bool publishMode = Helper::isBuildPublish();

        LOGGER_MESSAGE( "Sentry set extra [Publish: %u]"
            , publishMode
        );

        //sentry_set_extra( "Publish", sentry_value_new_bool( publishMode ) );

        const Char * ENGINE_GIT_SHA1 = Helper::getEngineGITSHA1();

        LOGGER_MESSAGE( "Sentry set extra [Engine Commit: %s]"
            , ENGINE_GIT_SHA1
        );

        //sentry_set_extra( "Engine Commit", sentry_value_new_string( ENGINE_GIT_SHA1 ) );

        const Char * BUILD_TIMESTAMP = Helper::getBuildTimestamp();

        LOGGER_MESSAGE( "Sentry set extra [Build Timestamp: %s]"
            , BUILD_TIMESTAMP
        );

        //sentry_set_extra( "Build Timestamp", sentry_value_new_string( BUILD_TIMESTAMP ) );

        const Char * BUILD_USERNAME = Helper::getBuildUsername();

        LOGGER_MESSAGE( "Sentry set extra [Build Username: %s]"
            , BUILD_USERNAME
        );

        //sentry_set_extra( "Build Username", sentry_value_new_string( BUILD_USERNAME ) );

        const Char * contentCommit = Helper::getContentCommit();

        LOGGER_MESSAGE( "Sentry set extra [Content Commit: %s]"
            , contentCommit
        );

        //sentry_set_extra( "Content Commit", sentry_value_new_string( contentCommit ) );

        if( HAS_OPTION( "sentrycrash" ) == true )
        {
            LOGGER_MESSAGE_RELEASE( "!!!test sentry crash!!!" );

            Char message_uid[21] = {'\0'};
            Helper::makeUID( 20, message_uid );
            message_uid[20] = '\0';

            LOGGER_MESSAGE_RELEASE( "uid: %s", message_uid );
            LOGGER_MESSAGE_RELEASE( "!!!test sentry crash!!!" );

            //sentry_value_t event = sentry_value_new_message_event( SENTRY_LEVEL_ERROR, "Test", "sentrycrash" );

            //sentry_capture_event( event );

            //Helper::crash( "sentrycrash" );
        }
    }
    //////////////////////////////////////////////////////////////////////////
}
