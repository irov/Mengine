#	pragma once

#	include <stdex/allocator.h>

namespace Menge
{
	namespace Helper
	{
		template<class T>
		T * allocateMemory( uint32_t _count )
		{
			size_t memory_size = sizeof(T) * _count;
			void * memory_buffer = stdex_malloc( memory_size );

			return reinterpret_cast<T *>(memory_buffer);
		}

		template<class T>
		T * reallocateMemory( void * _buffer, uint32_t _count )
		{
			size_t memory_size = sizeof(T) * _count;
			void * memory_buffer = stdex_realloc( _buffer, memory_size );

			return reinterpret_cast<T *>(memory_buffer);
		}

		inline void freeMemory( void * _memory )
		{
			stdex_free( _memory );
		}
	}
}