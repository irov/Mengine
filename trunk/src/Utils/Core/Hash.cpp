#	include "Hash.h"

namespace Menge
{
	namespace Helper
	{
		uint32_t makeHash( const void * _data, size_t _len )
		{
			if( _len == 0 )
			{
				return 0;
			}

			unsigned char * p = (unsigned char *)_data;
			long len = (long)_len;

			long x = 0;

			x ^= *p << 7;
			long it = len;
			while( --it >= 0 )
			{
				x = (1000003 * x) ^ *p++;
			}

			x ^= len;
			x ^= 0;

			if( x == -1 )
			{
				x = -2;
			}

			return (uint32_t)x;
		}
	}
}