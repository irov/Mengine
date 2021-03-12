#pragma once

#include "Kernel/Params.h"
#include "Kernel/UnicodeHelper.h"
#include "Kernel/Stringalized.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        template <class V>
        V getParam( const MapParams & _params, const ConstString & _key, V && _default )
        {
            typename MapParams::const_iterator it_found = _params.find( _key );

            if( it_found == _params.end() )
            {
                return _default;
            }

            const MapParams::mapped_type & value = it_found->second;

            return value;
        }
        //////////////////////////////////////////////////////////////////////////
        String getParam( const MapWParams & _params, const ConstString & _key, const Char * _default );
        WString getParam( const MapWParams & _params, const ConstString & _key, WString && _default );
        //////////////////////////////////////////////////////////////////////////
        template <class T>
        T getParam( const MapWParams & _params, const ConstString & _key, T && _default )
        {
            typename MapWParams::const_iterator it_found = _params.find( _key );

            if( it_found == _params.end() )
            {
                return _default;
            }

            const MapWParams::mapped_type & value = it_found->second;

            String utf8_value;
            Helper::unicodeToUtf8( value, &utf8_value );

            T type_value;
            Helper::stringalized( utf8_value.c_str(), &type_value );

            return type_value;
        }
        //////////////////////////////////////////////////////////////////////////
    }
}