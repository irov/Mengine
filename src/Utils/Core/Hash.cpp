#	include "Hash.h"

namespace Menge
{
	namespace Helper
	{
		long makeHash( const void * _data, size_t _len )
		{
			if( _len == 0 )
			{
				return 0;
			}

			const unsigned char * p = (const unsigned char *)_data;
			long len = (long)_len;

			long x = 0;

			x ^= *p << 7;
			long it = len;
			while( --it >= 0 )
			{
				x = (1000003 * x) ^ *p++;
			}

			x ^= len;

			if( x == -1 )
			{
				x = -2;
			}

			return x;
		}
	}
}