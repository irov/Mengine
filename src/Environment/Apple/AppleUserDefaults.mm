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
        bool AppleGetUserDefaultsString( const Char * _key, Char * const _value, size_t _capacity )
        {
            NSUserDefaults * defaults = [NSUserDefaults standardUserDefaults];
            
            if( defaults == nil )
            {
                return false;
            }
            
            NSString * _Nullable value = [defaults objectForKey:@(_key)];
            
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
            
            [defaults setObject:@(_value) forKey:@(_key)];
            
            [defaults synchronize];
            
            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool AppleGetUserDefaultsBoolean( const Char * _key, bool _default )
        {
            NSUserDefaults * defaults = [NSUserDefaults standardUserDefaults];
            
            if( defaults == nil )
            {
                return _default;
            }
            
            NSNumber * _Nullable value = [defaults objectForKey:@(_key)];
            
            if( value == nil )
            {
                return _default;
            }
            
            BOOL value_bool = [value boolValue];
                        
            return (bool)value_bool;
        }
        //////////////////////////////////////////////////////////////////////////
        bool AppleSetUserDefaultsBoolean( const Char * _key, bool _value )
        {
            NSUserDefaults * defaults = [NSUserDefaults standardUserDefaults];
            
            if( defaults == nil )
            {
                return false;
            }
            
            [defaults setObject:@(_value) forKey:@(_key)];
            
            [defaults synchronize];
            
            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool AppleRemoveUserDefaults( const Char * _key )
        {
            NSUserDefaults * defaults = [NSUserDefaults standardUserDefaults];
            
            if( defaults == nil )
            {
                return false;
            }

            [defaults removeObjectForKey:@(_key)];
            
            [defaults synchronize];

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
    }
}
