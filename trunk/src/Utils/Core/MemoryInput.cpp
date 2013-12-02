#	include "MemoryInput.h"

#	include <algorithm>

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	MemoryInput::MemoryInput()
		: m_data(nullptr)
		, m_pos(nullptr)
		, m_end(nullptr)
		, m_size(0)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	MemoryInput::~MemoryInput()
	{
        delete [] m_data;
        m_data = nullptr;
	}
	//////////////////////////////////////////////////////////////////////////
	void * MemoryInput::newMemory( size_t _size )
	{
        if( m_data != nullptr )
        {
            delete [] m_data;
            m_data = nullptr;
        }

		m_data = new unsigned char [_size];
		m_size = _size;
		m_pos = m_data;
		m_end = m_data + m_size;

        return m_data;
	}
	//////////////////////////////////////////////////////////////////////////
	void * MemoryInput::getMemory( size_t & _size ) const
	{
        _size = m_size;

		return m_data;
	}
	//////////////////////////////////////////////////////////////////////////
	size_t MemoryInput::read( void* _buf, size_t _count )
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

		std::copy( m_pos, m_pos + cnt, static_cast<unsigned char*>(_buf) );

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
	size_t MemoryInput::size() const 
	{
		return m_size;
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
