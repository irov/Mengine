#pragma once

#include "Interface/ConfigServiceInterface.h"

#include "Config/Metaprogramming.h"

namespace Mengine
{
    namespace Detail
    {
        template<class T>
        T getConfigValue( const Char * _section, const Char * _key, T _default )
        {
            if( SERVICE_IS_INITIALIZE( ConfigServiceInterface ) == false )
            {
                return _default;
            }

            const ConfigInterfacePtr & configDefault = CONFIG_SERVICE()
                ->getDefaultConfig();

            T value;
            configDefault->hasValue( _section, _key, _default, &value );

            return value;
        }
    }
}
//////////////////////////////////////////////////////////////////////////
#define HAS_CONFIG( section, key )\
    []( const Mengine::Char * _section, const Char * _key ) { static bool result = CONFIG_SERVICE()->getDefaultConfig()->existValue( _section, _key ); return result;}(section, key)
//////////////////////////////////////////////////////////////////////////
#define CONFIG_VALUE_BOOLEAN( section, key, d )\
    []( decltype(d) _default ) { static bool value = Mengine::Detail::getConfigValue( section, key, (bool)_default ); return value;}(d)
//////////////////////////////////////////////////////////////////////////
#define CONFIG_VALUE_INTEGER( section, key, d )\
    []( decltype(d) _default ) -> decltype(d) { static decltype(d) value = (decltype(d))Mengine::Detail::getConfigValue( section, key, (int64_t)_default ); return value;}(d)
//////////////////////////////////////////////////////////////////////////
#define CONFIG_VALUE_FLOAT( section, key, d )\
    []( decltype(d) _default ) -> decltype(d) { static decltype(d) value = (decltype(d))Mengine::Detail::getConfigValue( section, key, (double)_default ); return value;}(d)
//////////////////////////////////////////////////////////////////////////
#define CONFIG_VALUE_STRING( section, key, d )\
    []( const Mengine::Char * _default ) { static const Mengine::Char * value = Mengine::Detail::getConfigValue( section, key, _default ); return value;}(d)
//////////////////////////////////////////////////////////////////////////
#define CONFIG_VALUE_CONSTSTRING( section, key, d )\
    []( const Mengine::ConstString & _default ) { static Mengine::ConstString value = Mengine::Detail::getConfigValue( section, key, _default ); return value;}(d)
//////////////////////////////////////////////////////////////////////////
#define CONFIG_VALUE_FILEPATH( section, key, d )\
    []( const Mengine::FilePath & _default ) { static Mengine::FilePath value = Mengine::Detail::getConfigValue( section, key, _default ); return value;}(d)
//////////////////////////////////////////////////////////////////////////
#define CONFIG_VALUE_COLOR( section, key, d )\
    []( const Mengine::Color & _default ) { static Mengine::Color value = Mengine::Detail::getConfigValue( section, key, _default ); return value;}(d)
    //////////////////////////////////////////////////////////////////////////
#define CONFIG_VALUE_RESOLUTION( section, key, d )\
    []( const Mengine::Resolution & _default ) { static Mengine::Resolution value = Mengine::Detail::getConfigValue( section, key, _default ); return value;}(d)
////////////////////////////////////////////////////////////////////////////
#define CONFIG_VALUES( section, key, value )\
    CONFIG_SERVICE()->getDefaultConfig()->getValues( section, key, value )
////////////////////////////////////////////////////////////////////////////
