#include "AppleUtils.h"

#include "Kernel/StringCopy.h"

#include "Config/StdString.h"

#if defined(MENGINE_PLATFORM_MACOS)
#   import <AppKit/AppKit.h>
#else
#   import <UIKit/UIKit.h>
#endif

namespace Mengine
{
    namespace Helper
    {
        /////////////////////////////////////////////////////////////////////////
        void AppleLogMessage( const Char * _message )
        {
            NSLog( @"%s", @(_message) );
        }
        /////////////////////////////////////////////////////////////////////////
        void AppleLogFormat( const Char * _format, ... )
        {
			Char str[MENGINE_LOGGER_MAX_MESSAGE] = {'\0'};
			MENGINE_VA_LIST_TYPE args;
			MENGINE_VA_LIST_START( args, _format );
			NSLogv( @(_format), args );
			MENGINE_VA_LIST_END( args );
        }
        /////////////////////////////////////////////////////////////////////////
        void AppleLogFormatV( const Char * _format, MENGINE_VA_LIST_TYPE _va )
        {
            NSLogv( @(_format), _va );
        }
        /////////////////////////////////////////////////////////////////////////
        bool AppleOpenUrlInDefaultBrowser( const Char * _url )
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
        bool AppleGetUserDefaultsString( const Char * _key, Char * const _value, size_t _capacity )
        {
            NSUserDefaults * defaults = [NSUserDefaults standardUserDefaults];
            
            if( defaults == nil )
            {
                return false;
            }
            
            NSString * value = [defaults objectForKey:[[NSString alloc] initWithUTF8String:_key]];
            
            if( value == nil )
            {
                return false;
            }
            
            const Char * value_str = [value UTF8String];
            
            Helper::stringCopy( _value, value_str, _capacity );
            
            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool AppleSetUserDefaultsString( const Char * _key, const Char * _value )
        {
            NSUserDefaults * defaults = [NSUserDefaults standardUserDefaults];
            
            if( defaults == nil )
            {
                return false;
            }
            
            [defaults setObject:[[NSString alloc] initWithUTF8String:_value] forKey:[[NSString alloc] initWithUTF8String:_key]];
            
            [defaults synchronize];
            
            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool AppleRemoveUserDefaultsString( const Char * _key )
        {
            NSUserDefaults * defaults = [NSUserDefaults standardUserDefaults];
            
            if( defaults == nil )
            {
                return false;
            }

            [defaults removeObjectForKey:[[NSString alloc] initWithUTF8String:_key]];
            
            [defaults synchronize];

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
    }
}
