#include "AppleSentryService.h"

#include "Interface/PlatformServiceInterface.h"
#include "Interface/ApplicationInterface.h"
#include "Interface/LoggerServiceInterface.h"
#include "Interface/DateTimeSystemInterface.h"

#include "Environment/Apple/AppleBundle.h"

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
#include "Kernel/ConfigHelper.h"
#include "Kernel/TimestampHelper.h"

#include "Config/StdString.h"
#include "Config/StdIO.h"

#import <Sentry/Sentry.h>

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( AppleSentryService, Mengine::AppleSentryService );
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
        NOTIFICATION_ADDOBSERVERMETHOD_THIS( NOTIFICATOR_BOOTSTRAPPER_CREATE_APPLICATION, &AppleSentryService::notifyCreateApplication_, MENGINE_DOCUMENT_FACTORABLE );
        NOTIFICATION_ADDOBSERVERMETHOD_THIS( NOTIFICATOR_ASSERTION, &AppleSentryService::notifyAssertion_, MENGINE_DOCUMENT_FACTORABLE );
        NOTIFICATION_ADDOBSERVERMETHOD_THIS( NOTIFICATOR_ERROR, &AppleSentryService::notifyError_, MENGINE_DOCUMENT_FACTORABLE );
        NOTIFICATION_ADDOBSERVERMETHOD_THIS( NOTIFICATOR_ENGINE_STOP, &AppleSentryService::notifyEngineStop_, MENGINE_DOCUMENT_FACTORABLE );
        
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleSentryService::_finalizeService()
    {
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_BOOTSTRAPPER_CREATE_APPLICATION );
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_ASSERTION );
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_ERROR );
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_ENGINE_STOP );
        
        [SentrySDK close];
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleSentryService::notifyCreateApplication_()
    {
        [SentrySDK configureScope:^(SentryScope *_Nonnull scope) {
            NSString * bundleIdentifier = Helper::AppleGetBundleIdentifier();
            
            LOGGER_INFO_PROTECTED( "sentry", "sentry set extra [Application: %s]"
                , [bundleIdentifier UTF8String]
            );
            
            [scope setExtraValue:bundleIdentifier forKey:@"Application"];
            
            Char companyName[MENGINE_APPLICATION_COMPANY_MAXNAME + 1] = {'\0'};
            APPLICATION_SERVICE()
                ->getCompanyName( companyName );

            LOGGER_INFO_PROTECTED( "sentry", "sentry set extra [Company: %s]"
                , companyName
            );
            
            [scope setExtraValue:@(companyName) forKey:@"Company"];
            
            Char projectName[MENGINE_PLATFORM_PROJECT_TITLE_MAXNAME + 1] = {'\0'};
            APPLICATION_SERVICE()
                ->getProjectName( projectName );

            LOGGER_INFO_PROTECTED( "sentry", "sentry set extra [Project: %s]"
                , projectName
            );
            
            [scope setExtraValue:@(projectName) forKey:@"Project"];
            
#ifdef MENGINE_DEBUG
            Char userName[MENGINE_PLATFORM_USER_MAXNAME + 1] = {'\0'};
            PLATFORM_SERVICE()
                ->getUserName( userName );

            LOGGER_INFO_PROTECTED( "sentry", "sentry set extra [User: %s]"
                , userName
            );

            [scope setExtraValue:@(userName) forKey:@"User"];
#endif
            
            uint32_t projectVersion = APPLICATION_SERVICE()
                ->getProjectVersion();

            LOGGER_INFO_PROTECTED( "sentry", "sentry set extra [Version: %u]"
                , projectVersion
            );

            [scope setExtraValue:@(projectVersion) forKey:@"Version"];
            
            bool debugMode = Helper::isDebugMode();

            LOGGER_INFO_PROTECTED( "sentry", "sentry set extra [Debug: %u]"
                , debugMode
            );

            [scope setExtraValue:@(debugMode) forKey:@"Debug"];
            
            bool developmentMode = Helper::isDevelopmentMode();

            LOGGER_INFO_PROTECTED( "sentry", "sentry set extra [Development: %u]"
                , developmentMode
            );

            [scope setExtraValue:@(developmentMode) forKey:@"Development"];
            
            bool masterMode = Helper::isMasterRelease();

            LOGGER_INFO_PROTECTED( "sentry", "sentry set extra [Master: %u]"
                , masterMode
            );

            [scope setExtraValue:@(masterMode) forKey:@"Master"];
            
            bool publishMode = Helper::isBuildPublish();

            LOGGER_INFO_PROTECTED( "sentry", "sentry set extra [Publish: %u]"
                , publishMode
            );

            [scope setExtraValue:@(publishMode) forKey:@"Publish"];
            
            const Char * ENGINE_GIT_SHA1 = Helper::getEngineGITSHA1();

            LOGGER_INFO_PROTECTED( "sentry", "sentry set extra [Engine Commit: %s]"
                , ENGINE_GIT_SHA1
            );

            [scope setExtraValue:@(ENGINE_GIT_SHA1) forKey:@"Engine Commit"];
            
            const Char * BUILD_DATE = Helper::getBuildDate();

            LOGGER_INFO_PROTECTED( "sentry", "sentry set extra [Build Date: %s]"
                , BUILD_DATE
            );

            [scope setExtraValue:@(BUILD_DATE) forKey:@"Build Date"];
            
            const Char * BUILD_USERNAME = Helper::getBuildUsername();

            LOGGER_INFO_PROTECTED( "sentry", "sentry set extra [Build Username: %s]"
                , BUILD_USERNAME
            );

            [scope setExtraValue:@(BUILD_USERNAME) forKey:@"Build Username"];
            
            const Char * contentCommit = Helper::getContentCommit();

            LOGGER_INFO_PROTECTED( "sentry", "sentry set extra [Content Commit: %s]"
                , contentCommit
            );

            [scope setExtraValue:@(contentCommit) forKey:@"Content Commit"];
            
            Timestamp timestamp = Helper::getLocalTimestamp();

            Char LOG_DATE[256 + 1] = {'\0'};
            Helper::makeLoggerShortDate( timestamp, "%02u:%02u:%02u:%04u", LOG_DATE, 0, 256 );

            [scope setExtraValue:@(LOG_DATE) forKey:@"Log Date"];
            
            [scope setExtraValue:@NO forKey:@"Engine Stop"];
        }];
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleSentryService::notifyAssertion_( const Char * _category, EAssertionLevel _level, const Char * _test, const Char * _file, int32_t _line, const Char * _message )
    {
        MENGINE_UNUSED( _category );
        
        if( _level < ASSERTION_LEVEL_FATAL )
        {
            return;
        }

        [SentrySDK configureScope:^(SentryScope *_Nonnull scope) {
            [scope setExtraValue:@(_test) forKey:@"Assetion Test"];
            [scope setExtraValue:@(_file) forKey:@"Assetion Function"];
            [scope setExtraValue:@(_line) forKey:@"Assetion Line"];
        }];

        NSError *error = [NSError errorWithDomain:@(_message) code:0 userInfo: nil];
        [SentrySDK captureError:error];
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleSentryService::notifyError_( const Char * _category, EErrorLevel _level, const Char * _file, int32_t _line, const Char * _message )
    {
        MENGINE_UNUSED( _category );
        
        [SentrySDK configureScope:^(SentryScope *_Nonnull scope) {
            [scope setExtraValue:@(_level) forKey:@"Error Level"];
            [scope setExtraValue:@(_file) forKey:@"Error Function"];
            [scope setExtraValue:@(_line) forKey:@"Error Line"];
        }];

        NSError *error = [NSError errorWithDomain:@(_message) code:0 userInfo: nil];
        [SentrySDK captureError:error];
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleSentryService::notifyEngineStop_()
    {
        [SentrySDK configureScope:^(SentryScope *_Nonnull scope) {
            [scope setExtraValue:@YES forKey:@"Engine Stop"];
        }];
    }
    //////////////////////////////////////////////////////////////////////////
}
