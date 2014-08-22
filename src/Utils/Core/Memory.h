#	pragma once

#	include <stdex/allocator.h>

namespace Menge
{
	namespace Helper
	{
		template<class T>
		T * allocateMemory( size_t _count )
		{
			size_t memory_size = sizeof(T) * _count;
			void * memory_buffer = stdex_malloc( memory_size );

			return reinterpret_cast<T *>(memory_buffer);
		}

		template<class T>
		void deleteMemory( T * _memory )
		{
			stdex_free( _memory );
		}
	}
}