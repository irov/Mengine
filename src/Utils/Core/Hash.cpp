#	include "Hash.h"

namespace Menge
{
	namespace Helper
	{
		HashType makeHash( const void * _data, size_t _len )
		{
			if( _len == 0 )
			{
				return 0;
			}

			const unsigned char * p = (const unsigned char *)_data;

            HashType b = *p;

			HashType x = b << 7;

			for( size_t i = 0; i != _len; ++i )
			{
                HashType b2 = *p++;

				x = (1000003L * x) ^ b2;
			}

			x ^= (HashType)_len;

			if( x == -1 )
			{
				x = -2;
			}

			return x;
		}
	}
}