#include "MemoryBuffer.h"

#include "Core/MemoryAllocator.h"

#include "stdex/memorycopy.h"

namespace Mengine
{
	//////////////////////////////////////////////////////////////////////////
	MemoryBuffer::MemoryBuffer()
		: m_memory(nullptr)
		, m_size(0)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	MemoryBuffer::~MemoryBuffer()
	{
		Helper::freeMemory( m_memory );
		m_memory = nullptr;
	}
	//////////////////////////////////////////////////////////////////////////
	void MemoryBuffer::setMemory( const void * _ptr, size_t _size, const char * _file, uint32_t _line )
	{
		void * buffer = this->newMemory( _size, _file, _line );

		stdex::memorycopy( buffer, 0, _ptr, _size );
	}
	//////////////////////////////////////////////////////////////////////////
	Pointer MemoryBuffer::newMemory( size_t _size, const char * _file, uint32_t _line )
	{
		void * new_memory = Helper::reallocateMemory( m_memory, _size );

		if( new_memory == nullptr )
		{
			m_memory = nullptr;
			m_size = 0;

			return nullptr;
		}

		m_memory = new_memory;
		m_size = _size;
        m_file = _file;
        m_line = _line;

		return m_memory;
	}
}
