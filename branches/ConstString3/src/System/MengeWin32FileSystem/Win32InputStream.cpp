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
		: m_hFile(INVALID_HANDLE_VALUE)
		, m_size(0)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	Win32InputStream::~Win32InputStream()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool Win32InputStream::open( const WString& _filename )
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

        delete this;
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
	int Win32InputStream::tell() const
	{
		return ::SetFilePointer( m_hFile, 0, NULL, FILE_CURRENT );
	}
	//////////////////////////////////////////////////////////////////////////
	int Win32InputStream::size() const 
	{
		return static_cast<int>( m_size );
	}
	//////////////////////////////////////////////////////////////////////////
	static time_t s_FileTimeToUnixTime( const FILETIME * filetime )
	{
		unsigned int a0;			/* 16 bit, low    bits */
		unsigned int a1;			/* 16 bit, medium bits */
		unsigned int a2;			/* 32 bit, high   bits */

		unsigned int carry;		/* carry bit for subtraction */
		int negative;		/* whether a represents a negative value */

		/* Copy the time values to a2/a1/a0 */
		a2 = filetime->dwHighDateTime;
		a1 = ((unsigned int) filetime->dwLowDateTime) >> 16;
		a0 = ((unsigned int) filetime->dwLowDateTime) & 0xffff;

		/* Subtract the time difference */
		if (a0 >= 32768)
			a0 -= 32768, carry = 0;
		else
			a0 += (1 << 16) - 32768, carry = 1;

		if (a1 >= 54590 + carry)
			a1 -= 54590 + carry, carry = 0;
		else
			a1 += (1 << 16) - 54590 - carry, carry = 1;

		a2 -= 27111902 + carry;

		/* If a is negative, replace a by (-1-a) */
		negative = (a2 >= ((unsigned int) 1) << 31);
		if (negative)
		{
			/* Set a to -a - 1 (a is a2/a1/a0) */
			a0 = 0xffff - a0;
			a1 = 0xffff - a1;
			a2 = ~a2;
		}

		/* Divide a by 10000000 (a = a2/a1/a0), put the rest into r.
		Split the divisor into 10000 * 1000 which are both less than 0xffff. */
		a1 += (a2 % 10000) << 16;
		a2 /= 10000;
		a0 += (a1 % 10000) << 16;
		a1 /= 10000;
		a0 /= 10000;

		a1 += (a2 % 1000) << 16;
		a2 /= 1000;
		a0 += (a1 % 1000) << 16;
		a1 /= 1000;
		a0 /= 1000;

		/* If a was negative, replace a by (-1-a) and r by (9999999 - r) */
		if (negative)
		{
			/* Set a to -a - 1 (a is a2/a1/a0) */
			a0 = 0xffff - a0;
			a1 = 0xffff - a1;
			a2 = ~a2;
		}

		/* Do not replace this by << 32, it gives a compiler warning and it does
		not work. */
		return ((((time_t) a2) << 16) << 16) + (a1 << 16) + a0;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Win32InputStream::time( time_t & _time ) const
	{
		FILETIME creation;
		FILETIME access;
		FILETIME write;

		if( GetFileTime( m_hFile, &creation, &access, &write ) == FALSE )
		{
			return false;
		}

		_time = s_FileTimeToUnixTime( &write );

		return true;
	}
}	// namespace Menge
