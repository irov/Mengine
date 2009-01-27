/*
 *	FileStream.cpp
 *
 *	Created by _Berserk_ on 27.1.2009
 *	Copyright 2009 Menge. All rights reserved.
 *
 */

#	include "FileStream.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	FileStream::FileStream( HANDLE _handle )
		: m_handle( _handle )
		, m_readPointer( 0 )
	{
		m_size = GetFileSize( m_handle, NULL );
		if( m_size == INVALID_FILE_SIZE )
		{
			m_size = 0;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	FileStream::~FileStream()
	{
		CloseHandle( m_handle );
	}
	//////////////////////////////////////////////////////////////////////////
	void FileStream::release()
	{
		delete this;
	}
	//////////////////////////////////////////////////////////////////////////
	std::streamsize FileStream::read( void* _buf, std::streamsize _count )
	{
		DWORD bytesRead = 0;
		if( m_readPointer + _count > m_size )
		{
			_count = m_size - m_readPointer;
		}
		BOOL result = ReadFile( m_handle, _buf, _count, &bytesRead, NULL );
		m_readPointer += bytesRead;
		return bytesRead;
	}
	//////////////////////////////////////////////////////////////////////////
	void FileStream::seek( std::streamoff _pos )
	{
		DWORD newPos = SetFilePointer( m_handle, _pos, NULL, FILE_BEGIN );
		if( GetLastError() == NO_ERROR )
		{
			m_readPointer = newPos;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void FileStream::skip( std::streamoff _count )
	{
		DWORD newPos = SetFilePointer( m_handle, _count, NULL, FILE_CURRENT );
		if( GetLastError() == NO_ERROR )
		{
			m_readPointer = newPos;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	std::streampos FileStream::tell() const 
	{
		return m_readPointer;
	}
	//////////////////////////////////////////////////////////////////////////
	bool FileStream::eof() const 
	{
		return m_readPointer == m_size;
	}
	//////////////////////////////////////////////////////////////////////////
	std::streamsize FileStream::size() const 
	{
		return m_size;
	}
	//////////////////////////////////////////////////////////////////////////
	HANDLE FileStream::getHandle()
	{
		return m_handle;
	}
	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge
