#import "MacOSEnvironmentService.h"

#import "Interface/PlatformServiceInterface.h"

#import "Environment/Apple/AppleBundle.h"
#import "Environment/MacOS/MacOSApplication.h"

#import "Kernel/Logger.h"

#import "Config/StdString.h"

#import <Foundation/Foundation.h>
#import <SystemConfiguration/SystemConfiguration.h>
#import <Cocoa/Cocoa.h>

#include <sys/sysctl.h>
#include <unistd.h>

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( EnvironmentService, Mengine::MacOSEnvironmentService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    MacOSEnvironmentService::MacOSEnvironmentService()
        : m_installTimestamp( 0 )
        , m_installRND( 0 )
        , m_sessionTimestamp( 0 )
        , m_sessionIndex( 0 )
        , m_sessionRND( 0 )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    MacOSEnvironmentService::~MacOSEnvironmentService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool MacOSEnvironmentService::_initializeService()
    {
        Char hostName[MENGINE_ENVIRONMENT_USER_MAXNAME] = {'\0'};
        if( ::gethostname( hostName, MENGINE_ENVIRONMENT_USER_MAXNAME ) == 0 )
        {
            m_userName.assign( hostName );
        }

        Char deviceModel[MENGINE_ENVIRONMENT_DEVICE_MODEL_MAXNAME] = {'\0'};
        size_t deviceModelSize = MENGINE_ENVIRONMENT_DEVICE_MODEL_MAXNAME;
        if( ::sysctlbyname( "hw.model", deviceModel, &deviceModelSize, NULL, 0 ) == 0 )
        {
            m_deviceModel.assign( deviceModel );
        }

        NSString * language = [[NSLocale preferredLanguages] firstObject];
        m_deviceLanguage.assign( language != nil ? [language UTF8String] : "" );

        m_osFamily.assign( "MacOS" );

        NSString * systemVersion = [[NSProcessInfo processInfo] operatingSystemVersionString];
        m_osVersion.assign( systemVersion != nil ? [systemVersion UTF8String] : "" );

        NSString * bundleIdentifier = [AppleBundle getIdentifier];
        m_bundleId.assign( bundleIdentifier != nil ? [bundleIdentifier UTF8String] : "" );

        NSString * sessionId = [MacOSApplication.sharedInstance getSessionId];
        m_sessionId.assign( sessionId != nil ? [sessionId UTF8String] : "" );

        NSString * installId = [MacOSApplication.sharedInstance getInstallId];
        m_installId.assign( installId != nil ? [installId UTF8String] : "" );

        NSInteger installTimestamp = [MacOSApplication.sharedInstance getInstallTimestamp];
        m_installTimestamp = (Timestamp)installTimestamp;

        NSString * installVersion = [MacOSApplication.sharedInstance getInstallVersion];
        m_installVersion.assign( installVersion != nil ? [installVersion UTF8String] : "" );

        NSInteger installRND = [MacOSApplication.sharedInstance getInstallRND];
        m_installRND = (int64_t)installRND;

        NSInteger sessionTimestamp = [MacOSApplication.sharedInstance getSessionTimestamp];
        m_sessionTimestamp = (Timestamp)sessionTimestamp;

        NSInteger sessionIndex = [MacOSApplication.sharedInstance getSessionIndex];
        m_sessionIndex = (int64_t)sessionIndex;

        NSInteger sessionRND = [MacOSApplication.sharedInstance getSessionRND];
        m_sessionRND = (int64_t)sessionRND;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void MacOSEnvironmentService::_finalizeService()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void MacOSEnvironmentService::getUserName( Char * const _userName ) const
    {
        m_userName.copy( _userName );
    }
    //////////////////////////////////////////////////////////////////////////
    void MacOSEnvironmentService::getDeviceModel( Char * const _deviceModel ) const
    {
        m_deviceModel.copy( _deviceModel );
    }
    //////////////////////////////////////////////////////////////////////////
    void MacOSEnvironmentService::getDeviceLanguage( Char * const _deviceLanguage ) const
    {
        m_deviceLanguage.copy( _deviceLanguage );
    }
    //////////////////////////////////////////////////////////////////////////
    void MacOSEnvironmentService::getOSFamily( Char * const _osFamily ) const
    {
        m_osFamily.copy( _osFamily );
    }
    //////////////////////////////////////////////////////////////////////////
    void MacOSEnvironmentService::getOSVersion( Char * const _osVersion ) const
    {
        m_osVersion.copy( _osVersion );
    }
    //////////////////////////////////////////////////////////////////////////
    void MacOSEnvironmentService::getBundleId( Char * const _bundleId ) const
    {
        m_bundleId.copy( _bundleId );
    }
    //////////////////////////////////////////////////////////////////////////
    void MacOSEnvironmentService::getSessionId( Char * const _sessionId ) const
    {
        m_sessionId.copy( _sessionId );
    }
    //////////////////////////////////////////////////////////////////////////
    void MacOSEnvironmentService::getInstallId( Char * const _installId ) const
    {
        m_installId.copy( _installId );
    }
    //////////////////////////////////////////////////////////////////////////
    Timestamp MacOSEnvironmentService::getInstallTimestamp() const
    {
        return m_installTimestamp;
    }
    //////////////////////////////////////////////////////////////////////////
    void MacOSEnvironmentService::getInstallVersion( Char * const _installVersion ) const
    {
        m_installVersion.copy( _installVersion );
    }
    //////////////////////////////////////////////////////////////////////////
    int64_t MacOSEnvironmentService::getInstallRND() const
    {
        return m_installRND;
    }
    //////////////////////////////////////////////////////////////////////////
    Timestamp MacOSEnvironmentService::getSessionTimestamp() const
    {
        return m_sessionTimestamp;
    }
    //////////////////////////////////////////////////////////////////////////
    int64_t MacOSEnvironmentService::getSessionIndex() const
    {
        return m_sessionIndex;
    }
    //////////////////////////////////////////////////////////////////////////
    int64_t MacOSEnvironmentService::getSessionRND() const
    {
        return m_sessionRND;
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
    bool MacOSEnvironmentService::openMail( const Char * _email, const Char * _subject, const Char * _body, const Char * _technically )
    {
        NSString * email = [NSString stringWithUTF8String:_email];
        NSString * subject = [NSString stringWithUTF8String:_subject];
        NSString * body = [NSString stringWithUTF8String:_body];
        NSString * technically = [NSString stringWithUTF8String:_technically];

        MENGINE_UNUSED( technically );

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
