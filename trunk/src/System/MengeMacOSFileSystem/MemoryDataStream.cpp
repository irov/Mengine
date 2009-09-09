#	include "MemoryDataStream.h"

#	include <assert.h>

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	MemoryDataStream::MemoryDataStream( void* _pMem, std::streamsize _size )
		: m_size( _size )
	{
		m_data = m_pos = static_cast<unsigned char*>(_pMem);
		m_end = m_data + m_size;
	}
	//////////////////////////////////////////////////////////////////////////
	MemoryDataStream::~MemoryDataStream()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void MemoryDataStream::release()
	{
		delete this;
	}
	//////////////////////////////////////////////////////////////////////////
	std::streamsize MemoryDataStream::read( void* _buf, std::streamsize _count )
	{
		std::streamsize cnt = _count;
		// Read over end of memory?
		if ( m_pos + cnt > m_end )
		{
			cnt = m_end - m_pos;
		}

		if ( cnt == 0 )
		{
			return 0;
		}

		memcpy( _buf, m_pos, cnt );
		m_pos += cnt;
		return cnt;
	}
	//////////////////////////////////////////////////////////////////////////
	void MemoryDataStream::skip( std::streamoff _count )
	{
		std::streampos newpos = ( m_pos - m_data ) + _count;
		assert( m_data + newpos <= m_end );

		m_pos = m_data + newpos;
	}
	//////////////////////////////////////////////////////////////////////////
	void MemoryDataStream::seek( std::streamoff _pos )
	{
		assert( m_data + _pos <= m_end );
		m_pos = m_data + _pos;
	}
	//////////////////////////////////////////////////////////////////////////
	std::streampos MemoryDataStream::tell() const
	{
		//m_data is start, m_pos is current location
		return m_pos - m_data;
	}
	//////////////////////////////////////////////////////////////////////////
	bool MemoryDataStream::eof() const
	{
		return m_pos >= m_end;
	}
	//////////////////////////////////////////////////////////////////////////
	std::streamsize MemoryDataStream::size() const
	{
		return m_size;
	}
}
