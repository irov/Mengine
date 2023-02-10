#pragma once

#include "Config/Config.h"

namespace Mengine
{
    namespace Helper
    {
        template<class M, class K, class V>
        const V & getMapDefault( const M & _map, K && _key, V && _value )
        {
            typename M::const_iterator it_found = _map.find( _key );

            if( it_found == _map.end() )
            {
                return _value;
            }

            const V & value = it_found->second;

            return value;
        }
    }
}