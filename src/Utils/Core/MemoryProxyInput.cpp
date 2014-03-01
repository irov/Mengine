#	include "MemoryProxyInput.h"

#	include <algorithm>

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	MemoryProxyInput::MemoryProxyInput()
		: m_data(NULL)
		, m_pos(NULL)
		, m_end(NULL)
		, m_size(0)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	MemoryProxyInput::~MemoryProxyInput()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void MemoryProxyInput::setMemory( void * _memory, size_t _size )
	{
		m_data = static_cast<unsigned char*>( _memory );
		m_size = _size;
		m_pos = m_data;
		m_end = m_data + m_size;
	}
	//////////////////////////////////////////////////////////////////////////
	void * MemoryProxyInput::getMemory( size_t & _size ) const
	{
        _size = m_size;

		return m_data;
	}
	//////////////////////////////////////////////////////////////////////////
	size_t MemoryProxyInput::read( void* _buf, size_t _count )
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

		memcpy( static_cast<unsigned char*>(_buf), m_pos, cnt );
		
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
	size_t MemoryProxyInput::size() const 
	{
		return m_size;
	}
	//////////////////////////////////////////////////////////////////////////
	size_t MemoryProxyInput::tell() const
	{
        size_t distance = std::distance( m_data, m_pos );

		return distance;
	}
	//////////////////////////////////////////////////////////////////////////
	bool MemoryProxyInput::time( uint64_t & _time ) const
	{
        (void)_time;

		return false;
	}
}	// namespace Menge
