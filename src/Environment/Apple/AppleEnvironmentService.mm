#include "AppleEnvironmentService.h"

#include "Interface/StringizeServiceInterface.h"

#include "Config/StdString.h"

#if defined(MENGINE_PLATFORM_MACOS)
#   import <AppKit/AppKit.h>
#else
#   import <UIKit/UIKit.h>
#endif

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( AppleEnvironmentService, Mengine::AppleEnvironmentService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AppleEnvironmentService::AppleEnvironmentService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AppleEnvironmentService::~AppleEnvironmentService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleEnvironmentService::_initializeService()
    {
        //Empty

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleEnvironmentService::_finalizeService()
    {
        for( IntrusiveListConstStringHolderNSString::iterator
            it = m_holdersNSString.begin();
            it != m_holdersNSString.end(); )
        {
            IntrusiveListConstStringHolderNSString::iterator it_erase = it;

            ConstStringHolderNSString * holder = *it;
            ++it;

            m_holdersNSString.erase( it_erase );

            m_poolNSString.destroyT( holder );
        }

        m_holdersNSString.clear();
        m_poolNSString.clear();
    }
    //////////////////////////////////////////////////////////////////////////
    size_t AppleEnvironmentService::getDeviceName( Char * _deviceName, size_t _capacity ) const
    {
        NSString * name = [[UIDevice currentDevice] name];

        MENGINE_STRNCPY( _deviceName, [name UTF8String], _capacity );

        return name.length;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t AppleEnvironmentService::getDeviceModel( Char * _deviceModel, size_t _capacity ) const
    {
        NSString * platformString = [[UIDevice currentDevice] model];

        MENGINE_STRNCPY( _deviceModel, [platformString UTF8String], _capacity );

        return platformString.length;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t AppleEnvironmentService::getDeviceLanguage( Char * _deviceName, size_t _capacity ) const
    {
        NSString * language = [[NSLocale preferredLanguages] firstObject];

        MENGINE_STRNCPY( _deviceName, [language UTF8String], _capacity );

        return language.length;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t AppleEnvironmentService::getOSVersion( Char * _deviceName, size_t _capacity ) const
    {
        NSString * systemVersion = [[UIDevice currentDevice] systemVersion];

        MENGINE_STRNCPY( _deviceName, [systemVersion UTF8String], _capacity );

        return systemVersion.length;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t AppleEnvironmentService::getBundleId( Char * _bundleId, size_t _capacity ) const
    {
        NSString *bundleIdentifier = [[NSBundle mainBundle] bundleIdentifier];

        MENGINE_STRNCPY( _bundleId, [bundleIdentifier UTF8String], _capacity );

        return bundleIdentifier.length;
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleEnvironmentService::stringize( NSString * _value, ConstString * const _cstr )
    {
        if( [_value length] == 0 )
        {
            *_cstr = ConstString::none();

            return;
        }

        ConstStringHolderNSString * holder = m_poolNSString.createT();

        holder->setNSString( _value );

        if( STRINGIZE_SERVICE()
            ->stringizeExternal( holder, _cstr ) == false )
        {
            m_poolNSString.destroyT( holder );

            return;
        }

        m_holdersNSString.push_back( holder );
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleEnvironmentService::openUrlInDefaultBrowser( const Char * _url )
    {
        NSString * url = [NSString stringWithUTF8String: _url];

    #if defined(MENGINE_PLATFORM_MACOS)
        [[NSWorkspace sharedWorkspace] openURL:[NSURL URLWithString:url]];
    #else
        if( [[UIApplication sharedApplication] canOpenURL:[NSURL URLWithString:url]] == TRUE )
        {
            [[UIApplication sharedApplication] openURL:[NSURL URLWithString:url] options:@{} completionHandler:^(BOOL success)
            {
                //ToDo callback
            }];
        }
        else
        {
            return false;
        }
    #endif
        
        return true;
    }
    //////////////////////////////////////////////////////////////////////////    
}
