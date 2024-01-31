#include "Win32PreferencesSystem.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( PreferencesSystem, Mengine::Win32PreferencesSystem );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    Win32PreferencesSystem::Win32PreferencesSystem()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Win32PreferencesSystem::~Win32PreferencesSystem()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32PreferencesSystem::_initializeService()
    {
        //Empty

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32PreferencesSystem::_finalizeService()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    int64_t Win32PreferencesSystem::getPreferenceInteger( const Char * _key, int64_t _default ) const
    {
        MENGINE_UNUSED( _key );
        MENGINE_UNUSED( _default );

        //Empty

        return 0;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32PreferencesSystem::setPreferenceInteger( const Char * _key, int64_t _value )
    {
        MENGINE_UNUSED( _key );
        MENGINE_UNUSED( _value );

        //Empty

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32PreferencesSystem::getPreferenceString( const Char * _key, Char * const _default, size_t _capacity ) const
    {
        MENGINE_UNUSED( _key );
        MENGINE_UNUSED( _default );
        MENGINE_UNUSED( _capacity );

        //Empty

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32PreferencesSystem::setPreferenceString( const Char * _key, const Char * _value )
    {
        MENGINE_UNUSED( _key );
        MENGINE_UNUSED( _value );

        //Empty

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    ConstString Win32PreferencesSystem::getPreferenceConstString( const Char * _key, const ConstString & _default ) const
    {   
        MENGINE_UNUSED( _key );
        MENGINE_UNUSED( _default );

        //Empty

        return _default;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32PreferencesSystem::setPreferenceConstString( const Char * _key, const ConstString & _value )
    {
        MENGINE_UNUSED( _key );
        MENGINE_UNUSED( _value );

        //Empty

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32PreferencesSystem::hasPreference( const Char * _key ) const
    {
        MENGINE_UNUSED( _key );

        //Empty

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32PreferencesSystem::removePreference( const Char * _key )
    {
        MENGINE_UNUSED( _key );

        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
}
