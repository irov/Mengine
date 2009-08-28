/*
 *	MemoryStream.cpp
 *
 *	Created by _Berserk_ on 17.3.2009
 *	Copyright 2009 Menge. All rights reserved.
 *
 */

#	include "MemoryStream.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	MemoryStream::MemoryStream( void* _pMem, std::streamsize _size )
		: m_size( _size )
	{
		m_data = m_pos = static_cast<unsigned char*>( _pMem );
		m_end = m_data + m_size;
	}
	//////////////////////////////////////////////////////////////////////////
	MemoryStream::~MemoryStream()
	{

	}
	//////////////////////////////////////////////////////////////////////////
	void MemoryStream::release()
	{
		delete this;
	}
	//////////////////////////////////////////////////////////////////////////
	std::streamsize MemoryStream::read( void* _buf, std::streamsize _count )
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

		std::copy( m_pos, m_pos + cnt, (unsigned char*)_buf );
		m_pos += cnt;
		return cnt;
	}
	//////////////////////////////////////////////////////////////////////////
	void MemoryStream::skip( std::streamoff _count )
	{
		std::streamsize cnt = _count;
		// Read over end of memory?
		if ( m_pos + cnt > m_end )
		{
			cnt = m_end - m_pos;
		}

		m_pos += cnt;
	}
	//////////////////////////////////////////////////////////////////////////
	void MemoryStream::seek( std::streamoff _pos )
	{
		if( _pos < 0 )
		{
			_pos = 0;
		}
		else if( _pos > m_size )
		{
			_pos = m_size;
		}
		m_pos = m_data + _pos;
	}
	//////////////////////////////////////////////////////////////////////////
	std::streampos MemoryStream::tell() const 
	{
		return m_pos - m_data;
	}
	//////////////////////////////////////////////////////////////////////////
	bool MemoryStream::eof() const 
	{
		return m_pos >= m_end;
	}
	//////////////////////////////////////////////////////////////////////////
	std::streamsize MemoryStream::size() const 
	{
		return m_size;
	}
	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge
