#include "MacOSEnvironmentService.h"

#include "Interface/PlatformServiceInterface.h"

#include "Kernel/Logger.h"

#include "Config/StdString.h"

#import <UIKit/UIKit.h>
#import <MessageUI/MessageUI.h>

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( MacOSEnvironmentService, Mengine::MacOSEnvironmentService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    MacOSEnvironmentService::MacOSEnvironmentService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    MacOSEnvironmentService::~MacOSEnvironmentService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool MacOSEnvironmentService::_initializeService()
    {
        //Empty

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void MacOSEnvironmentService::_finalizeService()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    size_t MacOSEnvironmentService::getDeviceName( Char * const _deviceName, size_t _capacity ) const
    {
        NSString * name = [[UIDevice currentDevice] name];

        MENGINE_STRNCPY( _deviceName, [name UTF8String], _capacity );

        return name.length;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t MacOSEnvironmentService::getDeviceModel( Char * const _deviceModel, size_t _capacity ) const
    {
        NSString * platformString = [[UIDevice currentDevice] model];

        MENGINE_STRNCPY( _deviceModel, [platformString UTF8String], _capacity );

        return platformString.length;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t MacOSEnvironmentService::getDeviceLanguage( Char * const _deviceName, size_t _capacity ) const
    {
        NSString * language = [[NSLocale preferredLanguages] firstObject];

        MENGINE_STRNCPY( _deviceName, [language UTF8String], _capacity );

        return language.length;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t MacOSEnvironmentService::getOSVersion( Char * const _deviceName, size_t _capacity ) const
    {
        NSString * systemVersion = [[UIDevice currentDevice] systemVersion];

        MENGINE_STRNCPY( _deviceName, [systemVersion UTF8String], _capacity );

        return systemVersion.length;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t MacOSEnvironmentService::getBundleId( Char * const _bundleId, size_t _capacity ) const
    {
        NSString *bundleIdentifier = [[NSBundle mainBundle] bundleIdentifier];

        MENGINE_STRNCPY( _bundleId, [bundleIdentifier UTF8String], _capacity );

        return bundleIdentifier.length;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t MacOSEnvironmentService::getSessionId( Char * const _sessionId, size_t _capacity ) const
    {
        NSString * sessionId = [iOSApplication.sharedInstance getSessionId];
        
        MENGINE_STRNCPY( _sessionId, [sessionId UTF8String], _capacity );

        return sessionId.length;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t MacOSEnvironmentService::getInstallKey( Char * const _installKey, size_t _capacity ) const
    {
        NSString * installKey = [iOSApplication.sharedInstance getInstallKey];
        
        MENGINE_STRNCPY( _installKey, [installKey UTF8String], _capacity );

        return installKey.length;
    }
    //////////////////////////////////////////////////////////////////////////
    int64_t MacOSEnvironmentService::getInstallTimestamp() const
    {
        NSInteger installTimestamp = [iOSApplication.sharedInstance getInstallTimestamp];
        
        return installTimestamp;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t MacOSEnvironmentService::getInstallVersion( Char * const _installVersion, size_t _capacity ) const
    {
        NSString * installVersion = [iOSApplication.sharedInstance getInstallVersion];
        
        MENGINE_STRNCPY( _installVersion, [installVersion UTF8String], _capacity );

        return installVersion.length;
    }
    //////////////////////////////////////////////////////////////////////////
    int64_t MacOSEnvironmentService::getInstallRND() const
    {
        NSInteger installRND = [iOSApplication.sharedInstance getInstallRND];
        
        return installRND;
    }
    //////////////////////////////////////////////////////////////////////////
    int64_t MacOSEnvironmentService::getSessionIndex() const
    {
        NSInteger sessionIndex = [iOSApplication.sharedInstance getSessionIndex];
        
        return sessionIndex;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MacOSEnvironmentService::openUrlInDefaultBrowser( const Char * _url )
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
    bool MacOSEnvironmentService::openMail( const Char * _email, const Char * _subject, const Char * _body )
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
