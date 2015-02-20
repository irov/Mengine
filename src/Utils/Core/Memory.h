#	pragma once

#	include "Interface/ServiceInterface.h"
#	include "Interface/StreamInterface.h"

#   include "Factory/FactorablePtr.h"

#	include <stdex/allocator.h>

namespace Menge
{
	class Memory
		: public FactorablePtr
	{
	public:
		Memory();
		~Memory();

	public:
		void setServiceProvider( ServiceProviderInterface * _serviceProvider );

	public:
		void * newMemory( size_t _size );

	public:
		bool readStream( const InputStreamInterfacePtr & _stream );

	public:
		inline void * getMemory( size_t & _size ) const;
		inline size_t getSize() const;

	public:
		template<class T>
		inline T * getMemoryT( size_t & _size ) const
		{
			void * memory = this->getMemory( _size );

			return static_cast<T *>(memory);
		}

	protected:
		ServiceProviderInterface * m_serviceProvider;

		void * m_memory;
		size_t m_size;
	};
	//////////////////////////////////////////////////////////////////////////
	inline void * Memory::getMemory( size_t & _size ) const
	{
		_size = m_size;

		return m_memory;
	}
	//////////////////////////////////////////////////////////////////////////
	inline size_t Memory::getSize() const
	{
		return m_size;
	}
	//////////////////////////////////////////////////////////////////////////
	typedef stdex::intrusive_ptr<Memory> MemoryPtr;
	//////////////////////////////////////////////////////////////////////////
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