#pragma once

#include "Interface/ConfigServiceInterface.h"

#include "Config/Metaprogramming.h"

namespace Mengine
{
    namespace Helper
    {
        namespace Detail
        {
            //////////////////////////////////////////////////////////////////////////
            MENGINE_INLINE bool hasConfigValue( const Char * _section, const Char * _key )
            {
                if( SERVICE_IS_INITIALIZE( ConfigServiceInterface ) == false )
                {
                    return false;
                }

                const ConfigInterfacePtr & config = CONFIG_SERVICE()
                    ->getMainConfig();

                bool result = config->existValue( _section, _key );

                return result;
            }
            //////////////////////////////////////////////////////////////////////////
            template<class T>
            T getConfigValue( const Char * _section, const Char * _key, T _default )
            {
                if( SERVICE_IS_INITIALIZE( ConfigServiceInterface ) == false )
                {
                    return _default;
                }

                const ConfigInterfacePtr & config = CONFIG_SERVICE()
                    ->getMainConfig();

                T value;
                config->hasValue( _section, _key, _default, &value );

                return value;
            }
            //////////////////////////////////////////////////////////////////////////
            template<class T>
            void getConfigValues( const Char * _section, const Char * _key, T * const _values )
            {
                if( SERVICE_IS_INITIALIZE( ConfigServiceInterface ) == false )
                {
                    return;
                }

                const ConfigInterfacePtr & config = CONFIG_SERVICE()
                    ->getMainConfig();

                config->getValues( _section, _key, _values );
            }
            //////////////////////////////////////////////////////////////////////////
        }
        //////////////////////////////////////////////////////////////////////////
        ConfigInterfacePtr loadConfig( const ContentInterfacePtr & _content, const ConstString & _configType, const DocumentInterfacePtr & _doc );
        //////////////////////////////////////////////////////////////////////////
    }
}
//////////////////////////////////////////////////////////////////////////
#define HAS_CONFIG( section, key )\
    []( const Mengine::Char * _section, const Mengine::Char * _key ) { static bool result = Mengine::Helper::Detail::hasConfigValue( _section, _key ); return result;}(section, key)
//////////////////////////////////////////////////////////////////////////
#define CONFIG_VALUE_BOOLEAN( section, key, d )\
    []( decltype(d) _default ) { static bool value = Mengine::Helper::Detail::getConfigValue( section, key, (bool)_default ); return value;}(d)
//////////////////////////////////////////////////////////////////////////
#define CONFIG_VALUE_INTEGER( section, key, d )\
    []( decltype(d) _default ) -> decltype(d) { static decltype(d) value = (decltype(d))Mengine::Helper::Detail::getConfigValue( section, key, (int64_t)_default ); return value;}(d)
//////////////////////////////////////////////////////////////////////////
#define CONFIG_VALUE_FLOAT( section, key, d )\
    []( decltype(d) _default ) -> decltype(d) { static decltype(d) value = (decltype(d))Mengine::Helper::Detail::getConfigValue( section, key, (double)_default ); return value;}(d)
//////////////////////////////////////////////////////////////////////////
#define CONFIG_VALUE_STRING( section, key, d )\
    []( const Mengine::String & _default ) { Mengine::String value = Mengine::Helper::Detail::getConfigValue( section, key, _default ); return value;}(d)
//////////////////////////////////////////////////////////////////////////
#define CONFIG_VALUE_CONSTSTRING( section, key, d )\
    []( const Mengine::ConstString & _default ) { static Mengine::ConstString value = Mengine::Helper::Detail::getConfigValue( section, key, _default ); return value;}(d)
//////////////////////////////////////////////////////////////////////////
#define CONFIG_VALUE_FILEPATH( section, key, d )\
    []( const Mengine::FilePath & _default ) { static Mengine::FilePath value = Mengine::Helper::Detail::getConfigValue( section, key, _default ); return value;}(d)
//////////////////////////////////////////////////////////////////////////
#define CONFIG_VALUE_COLOR( section, key, d )\
    []( const Mengine::Color & _default ) { static Mengine::Color value = Mengine::Helper::Detail::getConfigValue( section, key, _default ); return value;}(d)
    //////////////////////////////////////////////////////////////////////////
#define CONFIG_VALUE_RESOLUTION( section, key, d )\
    []( const Mengine::Resolution & _default ) { static Mengine::Resolution value = Mengine::Helper::Detail::getConfigValue( section, key, _default ); return value;}(d)
////////////////////////////////////////////////////////////////////////////
#define CONFIG_VALUES( section, key, values )\
    Mengine::Helper::Detail::getConfigValues( section, key, values )
////////////////////////////////////////////////////////////////////////////
