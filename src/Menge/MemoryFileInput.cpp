/*
 *	MemoryFileInput.cpp
 *
 *	Created by _Berserk_ on 17.3.2009
 *	Copyright 2009 Menge. All rights reserved.
 *
 */

#	include "MemoryFileInput.h"

#	include <algorithm>

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	MemoryFileInput::MemoryFileInput()
		: m_data( NULL )
		, m_pos( NULL )
		, m_end( NULL )
		, m_size( 0 )
	{

	}
	//////////////////////////////////////////////////////////////////////////
	MemoryFileInput::~MemoryFileInput()
	{

	}
	//////////////////////////////////////////////////////////////////////////
	void MemoryFileInput::setMemory( void* _memPtr, int _size )
	{
		m_data = static_cast<unsigned char*>( _memPtr );
		m_size = _size;
		m_pos = m_data;
		m_end = m_data + m_size;
	}
	//////////////////////////////////////////////////////////////////////////
	void* MemoryFileInput::getMemory()
	{
		return m_data;
	}
	//////////////////////////////////////////////////////////////////////////
	int MemoryFileInput::read( void* _buf, int _count )
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
	void MemoryFileInput::seek( int _pos )
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
	int MemoryFileInput::size() const 
	{
		return m_size;
	}
	//////////////////////////////////////////////////////////////////////////
	int MemoryFileInput::tell()
	{
		return static_cast<int>(m_pos - m_data);
	}
	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge
