#	include "MemoryProxyInput.h"

#	include "stdex/memorycopy.h"

#	include <memory.h>

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	MemoryProxyInput::MemoryProxyInput()
		: m_data(nullptr)
		, m_size(0)
		, m_pos(nullptr)
		, m_end(nullptr)		
	{
	}
	//////////////////////////////////////////////////////////////////////////
	MemoryProxyInput::~MemoryProxyInput()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void * MemoryProxyInput::setMemory( void * _memory, size_t _offset, size_t _size )
	{
		m_data = static_cast<unsigned char*>( _memory ) + _offset;
		m_size = _size;

		m_pos = m_data;
		m_end = m_data + m_size;

		return m_data;
	}
	//////////////////////////////////////////////////////////////////////////
	void * MemoryProxyInput::getMemory( size_t & _size ) const
	{
        _size = m_size;

		return m_data;
	}
	//////////////////////////////////////////////////////////////////////////
	size_t MemoryProxyInput::read( void * _buf, size_t _count )
	{
		size_t cnt = _count;
		// Read over end of memory?
		if ( m_pos + cnt > m_end )
		{
			cnt = m_end - m_pos;
		}

		if ( cnt == 0 )
		{
			return 0;
		}

		stdex::memorycopy( _buf, m_pos, cnt );
		//std::copy( m_pos, m_pos + cnt, (unsigned char *)_buf );
		
		m_pos += cnt;

		return cnt;
	}
	//////////////////////////////////////////////////////////////////////////
	bool MemoryProxyInput::seek( size_t _pos )
	{
		if( _pos > m_size )
		{
			_pos = m_size;
		}

		m_pos = m_data + _pos;

        return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool MemoryProxyInput::skip( size_t _pos )
	{
		if( m_pos + _pos > m_end )
		{
			_pos = 0;
		}

		m_pos += _pos;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	size_t MemoryProxyInput::size() const 
	{
		return m_size;
	}
	//////////////////////////////////////////////////////////////////////////
	bool MemoryProxyInput::eof() const
	{
		return m_pos == m_end;
	}
	//////////////////////////////////////////////////////////////////////////
	size_t MemoryProxyInput::tell() const
	{
        size_t distance = m_pos - m_data;

		return distance;
	}
	//////////////////////////////////////////////////////////////////////////
	bool MemoryProxyInput::time( uint64_t & _time ) const
	{
        (void)_time;

		return false;
	}
}	// namespace Menge
