#	pragma once

#	include <stdint.h>
#	include <stddef.h>

namespace Menge
{
	typedef long HashType;

	namespace Helper
	{
		HashType makeHash( const void * _data, size_t _len );
	}
}