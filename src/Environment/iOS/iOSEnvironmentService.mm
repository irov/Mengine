#include "iOSEnvironmentService.h"

#include "Interface/PlatformServiceInterface.h"
#include "Interface/LoggerServiceInterface.h"

#import "Environment/iOS/iOSApplication.h"
#import "Environment/iOS/iOSDetail.h"

#include "Kernel/Logger.h"
#include "Kernel/FactorableUnique.h"

#include "Config/StdString.h"

#import <UIKit/UIKit.h>
#import <MessageUI/MessageUI.h>

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( iOSEnvironmentService, Mengine::iOSEnvironmentService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    iOSEnvironmentService::iOSEnvironmentService()
        : m_mailCompose( nil )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    iOSEnvironmentService::~iOSEnvironmentService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool iOSEnvironmentService::_initializeService()
    {
        iOSProxyLoggerPtr proxyLogger = Helper::makeFactorableUnique<iOSProxyLogger>(MENGINE_DOCUMENT_FACTORABLE);

        if( LOGGER_SERVICE()
            ->registerLogger( proxyLogger ) == true )
        {
            m_proxyLogger = proxyLogger;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void iOSEnvironmentService::_finalizeService()
    {
        if( m_proxyLogger != nullptr )
        {
            LOGGER_SERVICE()
                ->unregisterLogger( m_proxyLogger );

            m_proxyLogger = nullptr;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    size_t iOSEnvironmentService::getDeviceName( Char * const _deviceName, size_t _capacity ) const
    {
        NSString * name = [[UIDevice currentDevice] name];

        MENGINE_STRNCPY( _deviceName, [name UTF8String], _capacity );

        return name.length;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t iOSEnvironmentService::getDeviceModel( Char * const _deviceModel, size_t _capacity ) const
    {
        NSString * platformString = [[UIDevice currentDevice] model];

        MENGINE_STRNCPY( _deviceModel, [platformString UTF8String], _capacity );

        return platformString.length;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t iOSEnvironmentService::getDeviceLanguage( Char * const _deviceName, size_t _capacity ) const
    {
        NSString * language = [[NSLocale preferredLanguages] firstObject];

        MENGINE_STRNCPY( _deviceName, [language UTF8String], _capacity );

        return language.length;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t iOSEnvironmentService::getOSVersion( Char * const _deviceName, size_t _capacity ) const
    {
        NSString * systemVersion = [[UIDevice currentDevice] systemVersion];

        MENGINE_STRNCPY( _deviceName, [systemVersion UTF8String], _capacity );

        return systemVersion.length;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t iOSEnvironmentService::getBundleId( Char * const _bundleId, size_t _capacity ) const
    {
        NSString *bundleIdentifier = [[NSBundle mainBundle] bundleIdentifier];

        MENGINE_STRNCPY( _bundleId, [bundleIdentifier UTF8String], _capacity );

        return bundleIdentifier.length;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t iOSEnvironmentService::getSessionId( Char * const _sessionId, size_t _capacity ) const
    {
        NSString * sessionId = [iOSApplication.sharedInstance getSessionId];
        
        MENGINE_STRNCPY( _sessionId, [sessionId UTF8String], _capacity );

        return sessionId.length;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t iOSEnvironmentService::getInstallKey( Char * const _installKey, size_t _capacity ) const
    {
        NSString * installKey = [iOSApplication.sharedInstance getInstallKey];
        
        MENGINE_STRNCPY( _installKey, [installKey UTF8String], _capacity );

        return installKey.length;
    }
    //////////////////////////////////////////////////////////////////////////
    int64_t iOSEnvironmentService::getInstallTimestamp() const
    {
        NSInteger installTimestamp = [iOSApplication.sharedInstance getInstallTimestamp];
        
        return installTimestamp;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t iOSEnvironmentService::getInstallVersion( Char * const _installVersion, size_t _capacity ) const
    {
        NSString * installVersion = [iOSApplication.sharedInstance getInstallVersion];
        
        MENGINE_STRNCPY( _installVersion, [installVersion UTF8String], _capacity );

        return installVersion.length;
    }
    //////////////////////////////////////////////////////////////////////////
    int64_t iOSEnvironmentService::getInstallRND() const
    {
        NSInteger installRND = [iOSApplication.sharedInstance getInstallRND];
        
        return installRND;
    }
    //////////////////////////////////////////////////////////////////////////
    int64_t iOSEnvironmentService::getSessionIndex() const
    {
        NSInteger sessionIndex = [iOSApplication.sharedInstance getSessionIndex];
        
        return sessionIndex;
    }
    //////////////////////////////////////////////////////////////////////////
    bool iOSEnvironmentService::openUrlInDefaultBrowser( const Char * _url )
    {
        if( [[UIApplication sharedApplication] canOpenURL:[NSURL URLWithString:@(_url)]] == NO )
        {
            return false;
        }
        
        [[UIApplication sharedApplication] openURL:[NSURL URLWithString:@(_url)] options:@{} completionHandler:^(BOOL success) {
            //ToDo callback
        }];

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool iOSEnvironmentService::openMail( const Char * _email, const Char * _subject, const Char * _body )
    {
        UIViewController * view = Helper::iOSGetRootViewController();
        
        if( [iOSMailCompose canSendMail] == NO )
        {
            Helper::iOSAlert( view, @"Yikes.", @"Log into your mailbox using the standard Mail app" );
            
            return false;
        }
        
        iOSMailCompose * mailCompose = [[iOSMailCompose alloc] initWithViewController:view email:@(_email) subject:@(_subject) message:@(_body) completion:^{
            this->m_mailCompose = nullptr;
        }];

        m_mailCompose = mailCompose;
        
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}
