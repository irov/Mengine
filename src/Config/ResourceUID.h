#pragma once

#include "Config/HashType.h"
#include "Config/StdInt.h"

namespace Mengine
{
    struct ResourceUID
    {
        MENGINE_CONSTEXPR ResourceUID( uint64_t _high = MENGINE_UINT64_C(0), uint64_t _low = MENGINE_UINT64_C(0) )
            : high( _high )
            , low( _low )
        {
        }

        MENGINE_CONSTEXPR bool operator == ( const ResourceUID & _uid ) const
        {
            return high == _uid.high && low == _uid.low;
        }

        MENGINE_CONSTEXPR bool operator != ( const ResourceUID & _uid ) const
        {
            return this->operator == ( _uid ) == false;
        }

        uint64_t high;
        uint64_t low;
    };

    struct ResourceUIDHash
    {
        HashType operator()( const ResourceUID & _uid ) const
        {
            HashType hash = _uid.high;
            hash ^= _uid.low + MENGINE_UINT64_C(0x9e3779b97f4a7c15) + (hash << 6) + (hash >> 2);

            return hash;
        }
    };

    static MENGINE_CONSTEXPR ResourceUID INVALID_RESOURCE_UID = ResourceUID();
}
