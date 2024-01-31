#include "ApplePreferencesSystem.h"

#include "Environment/Apple/AppleUserDefaults.h"
#include "Environment/Apple/AppleString.h"

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
    int64_t ApplePreferencesSystem::getPreferenceInteger( const Char * _key, int64_t _default ) const
    {
        NSInteger apple_value = Helper::AppleGetUserDefaults( _key, (NSInteger)_default );

        int64_t value = (int64_t)apple_value;

        return value;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ApplePreferencesSystem::setPreferenceInteger( const Char * _key, int64_t _value )
    {
        NSInteger apple_value = (NSInteger)_value;

        if( Helper::AppleSetUserDefaultsInteger( _key, apple_value ) == NO )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    ConstString ApplePreferencesSystem::getPreferenceString( const Char * _key, const ConstString & _default ) const
    {
        NSString * apple_default = @(_default.c_str());

        NSString * apple_value = Helper::AppleGetUserDefaultsString( _key, apple_default );

        ConstString value = Helper::NSStringToConstString( apple_value );

        return value;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ApplePreferencesSystem::setPreferenceString( const Char * _key, const ConstString & _value )
    {
        NSString * apple_value = @(_default.c_str());

        if( Helper::AppleSetUserDefaultsString( _key, apple_value ) == NO )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}
