#include "iOSEnvironmentService.h"

#include "Interface/PlatformServiceInterface.h"
#include "Interface/LoggerServiceInterface.h"

#import "Environment/iOS/iOSApplication.h"
#import "Environment/iOS/iOSProxyLogger.h"
#import "Environment/iOS/iOSDetail.h"

#include "Config/StdString.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( EnvironmentService, Mengine::iOSEnvironmentService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    iOSEnvironmentService::iOSEnvironmentService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    iOSEnvironmentService::~iOSEnvironmentService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool iOSEnvironmentService::_initializeService()
    {
        NSString * deviceName = [[UIDevice currentDevice] name];
        
        m_userName.assign( [deviceName UTF8String] );
        
        NSString * deviceModel = [[UIDevice currentDevice] model];
        
        m_deviceModel.assign( [deviceModel UTF8String] );
        
        NSString * language = [[NSLocale preferredLanguages] firstObject];
        
        m_deviceLanguage.assign( [language UTF8String] );
        
        m_osFamily.assign( "iOS" );
        
        NSString * systemVersion = [[UIDevice currentDevice] systemVersion];
        
        m_osVersion.assign( [systemVersion UTF8String] );
        
        NSString * bundleIdentifier = [[NSBundle mainBundle] bundleIdentifier];
        
        m_bundleId.assign( [bundleIdentifier UTF8String] );
        
        NSString * sessionId = [iOSApplication.sharedInstance getSessionId];
        
        m_sessionId.assign( [sessionId UTF8String] );
        
        NSString * installKey = [iOSApplication.sharedInstance getInstallKey];
        
        m_installKey.assign( [installKey UTF8String] );
        
        NSInteger installRND = [iOSApplication.sharedInstance getInstallRND];
        
        m_installRND = (int64_t)installRND;
        
        NSInteger sessionIndex = [iOSApplication.sharedInstance getSessionIndex];
        
        m_sessionIndex = (int64_t)sessionIndex;
        
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
    int64_t iOSEnvironmentService::getInstallTimestamp() const
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
    int64_t iOSEnvironmentService::getSessionIndex() const
    {
        return m_sessionIndex;
    }
    //////////////////////////////////////////////////////////////////////////
}
