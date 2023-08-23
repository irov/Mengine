#pragma once

#include "Kernel/HashType.h"

#include "Config/Char.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        namespace Detail
        {
            //////////////////////////////////////////////////////////////////////////
            MENGINE_CONSTEXPR uint64_t xmul12864( uint64_t a, uint64_t b )
            {
                const uint64_t a32 = a >> 32;
                const uint64_t b32 = b >> 32;
                const uint32_t al = (uint32_t)a;
                const uint32_t ah = (uint32_t)a32;
                const uint32_t bl = (uint32_t)b;
                const uint32_t bh = (uint32_t)b32;

                uint64_t x1 = (uint64_t)(al)*bh;
                uint64_t x2 = (uint64_t)(ah)*bl;
                const uint64_t x3 = (uint64_t)(al) * (uint64_t)(bl);

                x2 += x3 >> 32;
                x1 += (uint32_t)(x2);

                const uint64_t x4 = (x1 << 32) | (uint32_t)(x3);

                return x4;
            }
            //////////////////////////////////////////////////////////////////////////
        }
        //////////////////////////////////////////////////////////////////////////
        MENGINE_CONSTEXPR HashType makeHash( const Char * _data, size_t _len )
        {
            if( _len == 0 )
            {
                return 0LL;
            }

            const Char * p = _data;

            HashType b = *p;

            HashType x = b << 7;

            for( size_t i = 0; i != _len; ++i )
            {
                HashType b2 = *p++;
                HashType x2 = Helper::Detail::xmul12864( 1000003ULL, x );

                x = x2 ^ b2;
            }

            x ^= (HashType)_len;

            if( x == ~0ULL )
            {
                x = ~1ULL;
            }

            return x;
        }
        //////////////////////////////////////////////////////////////////////////
        MENGINE_CONSTEXPR HashType combineHash( HashType hash1, HashType hash2 )
        {
            HashType hash = 0;

            hash ^= hash1 + 0x9e3779b9 + (hash << 6) + (hash >> 2);
            hash ^= hash2 + 0x9e3779b9 + (hash << 6) + (hash >> 2);

            return hash;
        }
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
    namespace Literals
    {
        //////////////////////////////////////////////////////////////////////////
        MENGINE_CONSTEXPR HashType operator "" _hash( const Char * _value, size_t _size )
        {
            return Helper::makeHash( _value, _size );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    using namespace Literals;
    //////////////////////////////////////////////////////////////////////////
}
//////////////////////////////////////////////////////////////////////////
using namespace Mengine::Literals;
//////////////////////////////////////////////////////////////////////////