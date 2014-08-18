#	include "MemoryInput.h"

#	include "stdex/memorycopy.h"
#	include <memory.h>

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	MemoryInput::MemoryInput()
		: m_data(nullptr)
		, m_size(0)
		, m_pos(nullptr)
		, m_end(nullptr)		
	{
	}
	//////////////////////////////////////////////////////////////////////////
	MemoryInput::~MemoryInput()
	{
        free( m_data );
        m_data = nullptr;
	}
	//////////////////////////////////////////////////////////////////////////
	void * MemoryInput::newMemory( size_t _size )
	{
		void * memory = realloc( m_data, _size );

		if( memory == nullptr )
		{
			return nullptr;
		}

		m_data = static_cast<unsigned char *>(memory);
		m_size = _size;
		m_pos = m_data;
		m_end = m_data + m_size;

        return m_data;
	}
	//////////////////////////////////////////////////////////////////////////
	const void * MemoryInput::getMemory( size_t & _size ) const		
	{
		_size = m_size;

		return m_data;
	}
	//////////////////////////////////////////////////////////////////////////
	size_t MemoryInput::read( void* _buf, size_t _count )
	{
		size_t cnt = _count;
		// Read over end of memory?
		if( m_pos + cnt > m_end )
		{
			cnt = m_end - m_pos;
		}

		if( cnt == 0 )
		{
			return 0;
		}

		stdex::memorycopy( _buf, m_pos, cnt );
		//std::copy( m_pos, m_pos + cnt, (unsigned char *)_buf );

		m_pos += cnt;

		return cnt;
	}
	//////////////////////////////////////////////////////////////////////////
	bool MemoryInput::seek( size_t _pos )
	{
		if( _pos > m_size )
		{
			_pos = m_size;
		}

		m_pos = m_data + _pos;

        return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool MemoryInput::skip( size_t _pos )
	{
		if( m_pos + _pos > m_end )
		{
			_pos = 0;
		}

		m_pos += _pos;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	size_t MemoryInput::size() const 
	{
		return m_size;
	}
	//////////////////////////////////////////////////////////////////////////
	bool MemoryInput::eof() const
	{
		return m_pos == m_end;
	}
	//////////////////////////////////////////////////////////////////////////
	size_t MemoryInput::tell() const
	{
        size_t distance = std::distance( m_data, m_pos );

		return distance;
	}
	//////////////////////////////////////////////////////////////////////////
	bool MemoryInput::time( uint64_t & _time ) const
	{
        (void)_time;

		return false;
	}
}	// namespace Menge
