/*
 *	Win32OutputStream.cpp
 *
 *	Created by _Berserk_ on 18.11.2009
 *	Copyright 2009 Menge. All rights reserved.
 *
 */

#	include "Win32OutputStream.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Win32OutputStream::Win32OutputStream()
		: m_hFile( INVALID_HANDLE_VALUE )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	Win32OutputStream::~Win32OutputStream()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool Win32OutputStream::open( const StringW& _filename )
	{
		DWORD shareAttrib = FILE_SHARE_READ | FILE_SHARE_WRITE;
		m_hFile = CreateFileW( _filename.c_str(),    // file to open
			GENERIC_WRITE,			// open for writing
			shareAttrib,			// share for reading, exclusive for mapping
			NULL,					// default security
			CREATE_ALWAYS,			// create always
			FILE_ATTRIBUTE_NORMAL,	// normal file
			NULL);					// no attr. template

		if ( m_hFile == INVALID_HANDLE_VALUE)
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Win32OutputStream::close()
	{
		if( m_hFile != INVALID_HANDLE_VALUE )
		{
			CloseHandle( m_hFile );
			m_hFile = INVALID_HANDLE_VALUE;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Win32OutputStream::write( const void * _data, std::streamsize _count )
	{
		DWORD bytesWritten = 0;
		::WriteFile( m_hFile, _data, _count, &bytesWritten, NULL );
	}
	//////////////////////////////////////////////////////////////////////////
	void Win32OutputStream::flush()
	{
		::FlushFileBuffers( m_hFile );
	}
	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge
