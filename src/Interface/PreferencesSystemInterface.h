#pragma once

#include "Interface/ServiceInterface.h"

#include "Kernel/ConstString.h"

#include "Config/Char.h"
#include "Config/StdInt.h"

namespace Mengine
{
    class PreferencesSystemInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "PreferencesSystem" )

    public:
        virtual bool setPreferenceInteger( const Char * _key, int64_t _value ) = 0;
        virtual int64_t getPreferenceInteger( const Char * _key, int64_t _default ) const = 0;
        
        virtual bool setPreferenceString( const Char * _key, const Char * _value ) = 0;
        virtual bool getPreferenceString( const Char * _key, Char * const _value, size_t _capacity, size_t * const _size ) const = 0;
        
        virtual bool setPreferenceConstString( const Char * _key, const ConstString & _value ) = 0;
        virtual ConstString getPreferenceConstString( const Char * _key, const ConstString & _default ) const = 0;
        
    public:
        virtual bool hasPreference( const Char * _key ) const = 0;
        virtual void removePreference( const Char * _key ) = 0;
    };
}
//////////////////////////////////////////////////////////////////////////
#define PREFERENCES_SYSTEM()\
    ((Mengine::PreferencesSystemInterface *)SERVICE_GET(Mengine::PreferencesSystemInterface))
//////////////////////////////////////////////////////////////////////////
