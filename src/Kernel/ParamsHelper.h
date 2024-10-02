#pragma once

#include "Kernel/Params.h"
#include "Kernel/UnicodeHelper.h"
#include "Kernel/Stringalized.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        String getParam( const Params & _params, const ConstString & _key, const Char * _default );
        WString getParam( const Params & _params, const ConstString & _key, const WChar * _default );
        //////////////////////////////////////////////////////////////////////////
        template<class T>
        T getParam( const Params & _params, const ConstString & _key, T && _default )
        {
            typename Params::const_iterator it_found = _params.find( _key );

            if( it_found == _params.end() )
            {
                return _default;
            }

            const Params::mapped_type & value = it_found->second;

            T type_value;
            Helper::get( value, &type_value );

            return type_value;
        }
        //////////////////////////////////////////////////////////////////////////
    }
}