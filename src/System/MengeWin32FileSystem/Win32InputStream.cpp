/*
 *	Win32InputStream.cpp
 *
 *	Created by _Berserk_ on 26.8.2009
 *	Copyright 2009 Menge. All rights reserved.
 *
 */

#	include "Win32InputStream.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Win32InputStream::Win32InputStream()
		: m_hFile( INVALID_HANDLE_VALUE )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	Win32InputStream::~Win32InputStream()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool Win32InputStream::open( const String& _filename )
	{
		DWORD shareAttrib = FILE_SHARE_READ;
		m_hFile = WindowsLayer::createFile( _filename,    // file to open
			GENERIC_READ,			// open for reading
			shareAttrib,			// share for reading, exclusive for mapping
			OPEN_EXISTING);

		if ( m_hFile == INVALID_HANDLE_VALUE)
		{
			return false;
		}

		m_size = ::GetFileSize( m_hFile, NULL );
		if( m_size == INVALID_FILE_SIZE )
		{
			m_size = 0;

			::CloseHandle( m_hFile );
			m_hFile = INVALID_HANDLE_VALUE;

			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Win32InputStream::close()
	{
		if( m_hFile != INVALID_HANDLE_VALUE )
		{
			::CloseHandle( m_hFile );
			m_hFile = INVALID_HANDLE_VALUE;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	int Win32InputStream::read( void* _buf, int _count )
	{
		DWORD bytesRead = 0;
		BOOL result = ::ReadFile( m_hFile, _buf, static_cast<DWORD>( _count ), &bytesRead, NULL );
		return static_cast<int>( bytesRead );
	}
	//////////////////////////////////////////////////////////////////////////
	void Win32InputStream::seek( int _pos )
	{
		::SetFilePointer( m_hFile, static_cast<LONG>( _pos ), NULL, FILE_BEGIN );
	}
	//////////////////////////////////////////////////////////////////////////
	int Win32InputStream::size() const 
	{
		return static_cast<int>( m_size );
	}
	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge
