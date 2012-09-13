/*
 *	MemoryFileInput.cpp
 *
 *	Created by _Berserk_ on 17.3.2009
 *	Copyright 2009 Menge. All rights reserved.
 *
 */

#	include "FileInputMemory.h"

#	include <algorithm>

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	FileInputMemory::FileInputMemory()
		: m_data( NULL )
		, m_pos( NULL )
		, m_end( NULL )
		, m_size( 0 )
	{

	}
	//////////////////////////////////////////////////////////////////////////
	FileInputMemory::~FileInputMemory()
	{

	}
	//////////////////////////////////////////////////////////////////////////
	void FileInputMemory::setMemory( void* _memPtr, int _size )
	{
		m_data = static_cast<unsigned char*>( _memPtr );
		m_size = _size;
		m_pos = m_data;
		m_end = m_data + m_size;
	}
	//////////////////////////////////////////////////////////////////////////
	void* FileInputMemory::getMemory()
	{
		return m_data;
	}
	//////////////////////////////////////////////////////////////////////////
	int FileInputMemory::read( void* _buf, int _count )
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

		std::copy( m_pos, m_pos + cnt, static_cast<unsigned char*>(_buf) );
		m_pos += cnt;
		return cnt;
	}
	//////////////////////////////////////////////////////////////////////////
	void FileInputMemory::seek( int _pos )
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
	int FileInputMemory::size() const 
	{
		return m_size;
	}
	//////////////////////////////////////////////////////////////////////////
	int FileInputMemory::tell() const
	{
		return std::distance( m_data, m_pos );
	}
	//////////////////////////////////////////////////////////////////////////
	bool FileInputMemory::time( time_t & _time ) const
	{
		return false;
	}
}	// namespace Menge
