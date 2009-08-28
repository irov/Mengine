/*
 *	MemoryStream.cpp
 *
 *	Created by _Berserk_ on 17.3.2009
 *	Copyright 2009 Menge. All rights reserved.
 *
 */

#	include "MemoryStream.h"

#	include <algorithm>

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	MemoryStream::MemoryStream( void* _pMem, int _size )
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
	int MemoryStream::read( void* _buf, int _count )
	{
		int cnt = _count;
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
	void MemoryStream::seek( int _pos )
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
	int MemoryStream::size() const 
	{
		return m_size;
	}
	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge
