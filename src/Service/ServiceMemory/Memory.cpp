#	include "Memory.h"

#	include "Core/MemoryAllocator.h"

#	include "stdex/memorycopy.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Memory::Memory()
		: m_memory(nullptr)
		, m_size(0)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	Memory::~Memory()
	{
		Helper::freeMemory( m_memory );
		m_memory = nullptr;
	}
	//////////////////////////////////////////////////////////////////////////
	void Memory::setMemory( const void * _ptr, size_t _size )
	{
		void * buffer = this->newMemory( _size );

		stdex::memorycopy( buffer, 0, _ptr, _size );
	}
	//////////////////////////////////////////////////////////////////////////
	Pointer Memory::newMemory( size_t _size )
	{
		unsigned char * new_memory = Helper::reallocateMemory<unsigned char>( m_memory, _size );

		if( new_memory == nullptr )
		{
			m_memory = nullptr;
			m_size = 0;

			return nullptr;
		}

		m_memory = new_memory;
		m_size = _size;

		return m_memory;
	}
}
