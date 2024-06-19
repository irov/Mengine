#include "MacOSEnvironmentService.h"

#include "Interface/PlatformServiceInterface.h"

#import "Environment/MacOS/MacOSApplication.h"

#include "Kernel/Logger.h"

#include "Config/StdString.h"

#import <Foundation/Foundation.h>
#import <SystemConfiguration/SystemConfiguration.h>
#import <Cocoa/Cocoa.h>

#import <sys/sysctl.h>

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( EnvironmentService, Mengine::MacOSEnvironmentService );
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
        ::gethostname( _deviceName, _capacity );
        
        size_t len = MENGINE_STRLEN( _deviceName );

        return len;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t MacOSEnvironmentService::getDeviceModel( Char * const _deviceModel, size_t _capacity ) const
    {
        size_t size;
        ::sysctlbyname("hw.model", NULL, &size, NULL, 0);
        
        if( size >= _capacity )
        {
            return 0;
        }
        
        ::sysctlbyname("hw.model", _deviceModel, &size, NULL, 0);

        return size;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t MacOSEnvironmentService::getDeviceLanguage( Char * const _deviceName, size_t _capacity ) const
    {
        NSString * language = [[NSLocale preferredLanguages] firstObject];

        MENGINE_STRNCPY( _deviceName, [language UTF8String], _capacity );

        return language.length;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t MacOSEnvironmentService::getOSFamily( Char * _osFamily, size_t _capacity ) const
    {
        size_t len = MENGINE_STRNCPY_STATIC( _osFamily, "MacOS", _capacity );

        return len;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t MacOSEnvironmentService::getOSVersion( Char * const _deviceName, size_t _capacity ) const
    {
        NSString * systemVersion = [[NSProcessInfo processInfo] operatingSystemVersionString];

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
        NSString * sessionId = [MacOSApplication.sharedInstance getSessionId];
        
        MENGINE_STRNCPY( _sessionId, [sessionId UTF8String], _capacity );

        return sessionId.length;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t MacOSEnvironmentService::getInstallKey( Char * const _installKey, size_t _capacity ) const
    {
        NSString * installKey = [MacOSApplication.sharedInstance getInstallKey];
        
        MENGINE_STRNCPY( _installKey, [installKey UTF8String], _capacity );

        return installKey.length;
    }
    //////////////////////////////////////////////////////////////////////////
    int64_t MacOSEnvironmentService::getInstallTimestamp() const
    {
        NSInteger installTimestamp = [MacOSApplication.sharedInstance getInstallTimestamp];
        
        return installTimestamp;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t MacOSEnvironmentService::getInstallVersion( Char * const _installVersion, size_t _capacity ) const
    {
        NSString * installVersion = [MacOSApplication.sharedInstance getInstallVersion];
        
        MENGINE_STRNCPY( _installVersion, [installVersion UTF8String], _capacity );

        return installVersion.length;
    }
    //////////////////////////////////////////////////////////////////////////
    int64_t MacOSEnvironmentService::getInstallRND() const
    {
        NSInteger installRND = [MacOSApplication.sharedInstance getInstallRND];
        
        return installRND;
    }
    //////////////////////////////////////////////////////////////////////////
    int64_t MacOSEnvironmentService::getSessionIndex() const
    {
        NSInteger sessionIndex = [MacOSApplication.sharedInstance getSessionIndex];
        
        return sessionIndex;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MacOSEnvironmentService::openUrlInDefaultBrowser( const Char * _url )
    {
        NSString * urlString = [NSString stringWithUTF8String:_url];
        
        NSURL * url = [NSURL URLWithString:urlString];
        
        if( url == nil )
        {
            return false;
        }
        
        if( [[NSWorkspace sharedWorkspace] openURL:url] == NO )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MacOSEnvironmentService::openMail( const Char * _email, const Char * _subject, const Char * _body )
    {
        NSString * email = [NSString stringWithUTF8String:_email];
        NSString * subject = [NSString stringWithUTF8String:_subject];
        NSString * body = [NSString stringWithUTF8String:_body];
        
        NSString * mailString = [NSString stringWithFormat:@"mailto:%@?subject=%@&body=%@"
                                , email
                                , [subject stringByAddingPercentEncodingWithAllowedCharacters:[NSCharacterSet URLQueryAllowedCharacterSet]]
                                , [body stringByAddingPercentEncodingWithAllowedCharacters:[NSCharacterSet URLQueryAllowedCharacterSet]]];
        
        NSURL *url = [NSURL URLWithString:mailString];
        
        if( url == nil )
        {
            return false;
        }
        
        if( [[NSWorkspace sharedWorkspace] openURL:url] == NO )
        {
            return false;
        }
        
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}
