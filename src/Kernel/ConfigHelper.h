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
#define CONFIG_EXIST( section, key )\
    []() { static bool result = CONFIG_SERVICE()->getDefaultConfig()->existValue( section, key); return result;}()
//////////////////////////////////////////////////////////////////////////
#define CONFIG_VALUE( section, key, default )\
    []() -> const decltype(Mengine::mpl::decltype_string(default)) &{ static decltype(Mengine::mpl::decltype_string( default )) value = Mengine::Detail::getConfigValue( section, key, default); return value;}()
//////////////////////////////////////////////////////////////////////////
#define CONFIG_VALUET( section, key, default, type )\
    []() -> const type &{static type value = Mengine::Detail::getConfigValue( section, key, (type)default); return value;}()
////////////////////////////////////////////////////////////////////////////
#define CONFIG_VALUES( section, key, value )\
    CONFIG_SERVICE()->getDefaultConfig()->getValues( section, key, value )
////////////////////////////////////////////////////////////////////////////
