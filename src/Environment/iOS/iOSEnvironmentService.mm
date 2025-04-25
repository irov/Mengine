#include "iOSEnvironmentService.h"

#include "Interface/PlatformServiceInterface.h"
#include "Interface/LoggerServiceInterface.h"

#import "Environment/Apple/AppleBundle.h"

#import "Environment/iOS/iOSApplication.h"
#import "Environment/iOS/iOSDetail.h"

#include "Config/StdString.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( EnvironmentService, Mengine::iOSEnvironmentService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    iOSEnvironmentService::iOSEnvironmentService()
        : m_installTimestamp( -1 )
        , m_installRND( -1 )
        , m_sessionTimestamp( -1 )
        , m_sessionIndex( -1 )
        , m_sessionRND( -1 )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    iOSEnvironmentService::~iOSEnvironmentService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool iOSEnvironmentService::_initializeService()
    {
        NSString * deviceName = [iOSDetail getDeviceName];
        
        m_userName.assign( [deviceName UTF8String] );
        
        NSString * deviceModel = [iOSDetail getDeviceModel];
        
        m_deviceModel.assign( [deviceModel UTF8String] );
        
        NSString * language = [[NSLocale preferredLanguages] firstObject];
        
        m_deviceLanguage.assign( [language UTF8String] );
        
        m_osFamily.assign( "iOS" );
        
        NSString * systemVersion = [iOSDetail getSystemVersion];
        
        m_osVersion.assign( [systemVersion UTF8String] );
        
        NSString * bundleIdentifier = [AppleBundle getIdentifier];
        
        m_bundleId.assign( [bundleIdentifier UTF8String] );
        
        NSString * sessionId = [iOSApplication.sharedInstance getSessionId];
        
        m_sessionId.assign( [sessionId UTF8String] );
        
        NSInteger installTimestamp = [iOSApplication.sharedInstance getInstallTimestamp];
        
        m_installTimestamp = (Timestamp)installTimestamp;
        
        NSString * installKey = [iOSApplication.sharedInstance getInstallKey];
        
        m_installKey.assign( [installKey UTF8String] );
        
        NSInteger installRND = [iOSApplication.sharedInstance getInstallRND];
        
        m_installRND = (int64_t)installRND;
        
        NSInteger sessionTimestamp = [iOSApplication.sharedInstance getSessionTimestamp];
        
        m_sessionTimestamp = (Timestamp)sessionTimestamp;
        
        NSInteger sessionIndex = [iOSApplication.sharedInstance getSessionIndex];
        
        m_sessionIndex = (int64_t)sessionIndex;
        
        NSInteger sessionRND = [iOSApplication.sharedInstance getSessionRND];
        
        m_sessionRND = (int64_t)sessionRND;
        
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void iOSEnvironmentService::_finalizeService()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void iOSEnvironmentService::getUserName( Char * const _userName ) const
    {
        m_userName.copy( _userName );
    }
    //////////////////////////////////////////////////////////////////////////
    void iOSEnvironmentService::getDeviceModel( Char * const _deviceModel ) const
    {
        m_deviceModel.copy( _deviceModel );
    }
    //////////////////////////////////////////////////////////////////////////
    void iOSEnvironmentService::getDeviceLanguage( Char * const _deviceLanguage ) const
    {
        m_deviceLanguage.copy( _deviceLanguage );
    }
    //////////////////////////////////////////////////////////////////////////
    void iOSEnvironmentService::getOSFamily( Char * const _osFamily ) const
    {
        m_osFamily.copy( _osFamily );
    }
    //////////////////////////////////////////////////////////////////////////
    void iOSEnvironmentService::getOSVersion( Char * const _osVersion ) const
    {
        m_osVersion.copy( _osVersion );
    }
    //////////////////////////////////////////////////////////////////////////
    void iOSEnvironmentService::getBundleId( Char * const _bundleId ) const
    {
        m_bundleId.copy( _bundleId );
    }
    //////////////////////////////////////////////////////////////////////////
    void iOSEnvironmentService::getSessionId( Char * const _sessionId ) const
    {
        m_sessionId.copy( _sessionId );
    }
    //////////////////////////////////////////////////////////////////////////
    void iOSEnvironmentService::getInstallKey( Char * const _installKey ) const
    {
        m_installKey.copy( _installKey );
    }
    //////////////////////////////////////////////////////////////////////////
    Timestamp iOSEnvironmentService::getInstallTimestamp() const
    {
        return m_installTimestamp;
    }
    //////////////////////////////////////////////////////////////////////////
    void iOSEnvironmentService::getInstallVersion( Char * const _installVersion ) const
    {
        m_installVersion.copy( _installVersion );
    }
    //////////////////////////////////////////////////////////////////////////
    int64_t iOSEnvironmentService::getInstallRND() const
    {
        return m_installRND;
    }
    //////////////////////////////////////////////////////////////////////////
    Timestamp iOSEnvironmentService::getSessionTimestamp() const
    {
        return m_sessionTimestamp;
    }
    //////////////////////////////////////////////////////////////////////////
    int64_t iOSEnvironmentService::getSessionIndex() const
    {
        return m_sessionIndex;
    }
    ///////////////////////////////////////////////////////////////////////////
    int64_t iOSEnvironmentService::getSessionRND() const
    {
        return m_sessionRND;
    }
    //////////////////////////////////////////////////////////////////////////
}
