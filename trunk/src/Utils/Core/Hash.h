#	pragma once

#	include <stdint.h>
#	include <stddef.h>

namespace Menge
{
	namespace Helper
	{
		uint32_t makeHash( const void * _data, size_t _len );	
	}
}