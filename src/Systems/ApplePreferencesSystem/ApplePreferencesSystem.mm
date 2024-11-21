#include "ApplePreferencesSystem.h"

#import "Environment/Apple/AppleUserDefaults.h"
#import "Environment/Apple/AppleString.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( PreferencesSystem, Mengine::ApplePreferencesSystem );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    ApplePreferencesSystem::ApplePreferencesSystem()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ApplePreferencesSystem::~ApplePreferencesSystem()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool ApplePreferencesSystem::_initializeService()
    {
        //Empty

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ApplePreferencesSystem::_finalizeService()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    bool ApplePreferencesSystem::setPreferenceInteger( const Char * _key, int64_t _value )
    {
        NSString * apple_key = [@("mengine.") stringByAppendingString:@(_key)];
        NSInteger apple_value = (NSInteger)_value;

        if( [AppleUserDefaults setIntegerForKey:apple_key value:apple_value] == NO )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    int64_t ApplePreferencesSystem::getPreferenceInteger( const Char * _key, int64_t _default ) const
    {
        NSString * apple_key = [@("mengine.") stringByAppendingString:@(_key)];
        NSInteger apple_default = (NSInteger)_default;

        NSInteger apple_value = [AppleUserDefaults getIntegerForKey:apple_key defaultValue:apple_default];

        int64_t value = (int64_t)apple_value;

        return value;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ApplePreferencesSystem::setPreferenceString( const Char * _key, const Char * _value )
    {
        NSString * apple_key = [@("mengine.") stringByAppendingString:@(_key)];
        NSString * apple_value = @(_value);

        if( [AppleUserDefaults setStringForKey:apple_key value:apple_value] == NO )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ApplePreferencesSystem::getPreferenceString( const Char * _key, Char * const _value, size_t _capacity, size_t * const _size ) const
    {
        NSString * apple_key = [@("mengine.") stringByAppendingString:@(_key)];
        
        NSString * apple_value = [AppleUserDefaults getStringForKey:apple_key defaultValue:nil];

        if( apple_value == nil )
        {
            return false;
        }

        const Char * value_str = [apple_value UTF8String];

        Helper::stringCopy( _value, value_str, _capacity );
        
        if( _size != nullptr )
        {
            *_size = apple_value.length;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ApplePreferencesSystem::setPreferenceConstString( const Char * _key, const ConstString & _value )
    {
        NSString * apple_key = [@("mengine.") stringByAppendingString:@(_key)];
        NSString * apple_value = @(_value.c_str());

        if( [AppleUserDefaults setStringForKey:apple_key value:apple_value] == NO )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    ConstString ApplePreferencesSystem::getPreferenceConstString( const Char * _key, const ConstString & _default ) const
    {
        NSString * apple_key = [@("mengine.") stringByAppendingString:@(_key)];
        NSString * apple_default = @(_default.c_str());

        NSString * apple_value = [AppleUserDefaults getStringForKey:apple_key defaultValue:apple_default];

        ConstString value = [AppleString NSStringToConstString:apple_value];

        return value;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ApplePreferencesSystem::hasPreference( const Char * _key ) const
    {
        NSString * apple_key = [@("mengine.") stringByAppendingString:@(_key)];

        BOOL result = [AppleUserDefaults hasKey:apple_key];

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    void ApplePreferencesSystem::removePreference( const Char * _key )
    {
        NSString * apple_key = [@("mengine.") stringByAppendingString:@(_key)];

        [AppleUserDefaults removeKey:apple_key];
    }
    //////////////////////////////////////////////////////////////////////////
}
