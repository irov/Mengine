#include "AppleUserDefaults.h"

#include "Kernel/StringCopy.h"

#if defined(MENGINE_PLATFORM_MACOS)
#   import <AppKit/AppKit.h>
#else
#   import <UIKit/UIKit.h>
#endif

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        NSString * AppleGetUserDefaultsString( NSString * _key, NSString * _default )
        {
            NSUserDefaults * defaults = [NSUserDefaults standardUserDefaults];
            
            if( defaults == nil )
            {
                return _default;
            }
            
            if( [defaults objectForKey:_key] == nil )
            {
                return _default;
            }
            
            NSString * value = [defaults stringForKey:_key];
            
            return value;
        }
        //////////////////////////////////////////////////////////////////////////
        BOOL AppleSetUserDefaultsString( NSString * _key, NSString * _value )
        {
            NSUserDefaults * defaults = [NSUserDefaults standardUserDefaults];
            
            if( defaults == nil )
            {
                return NO;
            }
            
            [defaults setObject:_value forKey:_key];
            
            [defaults synchronize];
            
            return YES;
        }
        //////////////////////////////////////////////////////////////////////////
        BOOL AppleGetUserDefaultsBoolean( NSString * _key, BOOL _default )
        {
            NSUserDefaults * defaults = [NSUserDefaults standardUserDefaults];
            
            if( defaults == nil )
            {
                return _default;
            }
            
            if( [defaults objectForKey:_key] == nil )
            {
                return _default;
            }
            
            BOOL value = [defaults boolForKey:_key];
            
            return value;
        }
        //////////////////////////////////////////////////////////////////////////
        BOOL AppleSetUserDefaultsBoolean( NSString * _key, BOOL _value )
        {
            NSUserDefaults * defaults = [NSUserDefaults standardUserDefaults];
            
            if( defaults == nil )
            {
                return NO;
            }
            
            [defaults setBool:_value forKey:_key];
            
            [defaults synchronize];
            
            return YES;
        }
        //////////////////////////////////////////////////////////////////////////
        NSInteger AppleGetUserDefaultsInteger( NSString * _key, NSInteger _default )
        {
            NSUserDefaults * defaults = [NSUserDefaults standardUserDefaults];
        
            if( defaults == nil )
            {
                return _default;
            }
            
            if( [defaults objectForKey:_key] == nil )
            {
                return _default;
            }
        
            NSInteger value = [defaults integerForKey:_key];
        
            return value;
        }
        //////////////////////////////////////////////////////////////////////////
        BOOL AppleSetUserDefaultsInteger( NSString * _key, NSInteger _value )
        {
            NSUserDefaults * defaults = [NSUserDefaults standardUserDefaults];
        
            if( defaults == nil )
            {
                return NO;
            }
        
            [defaults setInteger:_value forKey:_key];
        
            [defaults synchronize];
        
            return YES;
        }
        //////////////////////////////////////////////////////////////////////////
        BOOL AppleHasUserDefaults( NSString * _key )
        {
            NSUserDefaults * defaults = [NSUserDefaults standardUserDefaults];
            
            if( defaults == nil )
            {
                return NO;
            }
            
            if( [defaults objectForKey:_key] == nil )
            {
                return NO;
            }
            
            return YES;
        }
        //////////////////////////////////////////////////////////////////////////
        BOOL AppleRemoveUserDefaults( NSString * _key )
        {
            NSUserDefaults * defaults = [NSUserDefaults standardUserDefaults];
            
            if( defaults == nil )
            {
                return NO;
            }

            [defaults removeObjectForKey:_key];
            
            [defaults synchronize];

            return YES;
        }
        //////////////////////////////////////////////////////////////////////////
    }
}
