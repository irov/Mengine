#pragma once

#include "Config/Char.h"

#include <stdint.h>
#include <stddef.h>

namespace Mengine
{
	typedef int64_t HashType;

	namespace Helper
	{
        constexpr uint64_t xmul12864( uint64_t a, uint64_t b )
        {
            const uint32_t al = (uint32_t)a;
            const uint32_t ah = (uint32_t)(a >> 32);
            const uint32_t bl = (uint32_t)b;
            const uint32_t bh = (uint32_t)(b >> 32);

            uint64_t x1 = (uint64_t)(al)* bh;
            uint64_t x2 = (uint64_t)(ah)* bl;
            const uint64_t x3 = (uint64_t)(al)* bl;

            x2 += x3 >> 32;
            x1 += (uint32_t)(x2);

            const uint64_t x4 = (x1 << 32) | (uint32_t)(x3);

            return x4;
        }

        constexpr HashType makeHash( const Char * _data, const HashType _len )
        {
            if( _len == 0 )
            {
                return 0LL;
            }

            const Char * p = _data;

            const HashType b = *p;

            HashType x = b << 7;

            for( HashType i = 0; i != _len; ++i )
            {
                HashType b2 = *p++;
                HashType x2 = xmul12864( 1000003ULL, x );

                x = x2 ^ b2;
            }

            x ^= _len;

            if( x == -1 )
            {
                x = -2;
            }

            return x;
        }
	}
}