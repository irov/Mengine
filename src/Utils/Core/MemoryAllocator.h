#	pragma once

#	include <stdex/allocator.h>

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	class MemoryAllocator
	{ 
	public:
		inline void * operator new (size_t _size)
		{
			return stdex_malloc( _size );
		}

		inline void operator delete (void * _ptr, size_t _size)
		{
			(void)_size;

			stdex_free( _ptr );
		}

		inline void * operator new []( size_t _size )
		{
			return stdex_malloc( _size );
		}

			inline void operator delete []( void * _ptr, size_t _size )
		{
			(void)_size;

			stdex_free( _ptr );
		}
	};
	//////////////////////////////////////////////////////////////////////////
	namespace Helper
	{
		template<class T>
		T * allocateT()
		{
			size_t memory_size = sizeof( T );
			void * memory_buffer = stdex_malloc( memory_size );

            new (memory_buffer)T();

			return reinterpret_cast<T *>(memory_buffer);
		}

		template<class T>
		void freeT( T * _t )
		{
			_t->~T();
			
			stdex_free( _t );
		}

		template<class T>
		T * allocateMemory( size_t _count )
		{
			size_t memory_size = sizeof(T) * _count;
			void * memory_buffer = stdex_malloc( memory_size );

			return reinterpret_cast<T *>(memory_buffer);
		}

		template<class T>
		T * reallocateMemory( void * _buffer, size_t _count )
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