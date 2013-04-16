/*
 *	PosixInputStream.cpp
 *
 *	Created by _Berserk_ on 7.10.2009
 *	Copyright 2009 Menge. All rights reserved.
 *
 */

#	include "PosixInputStream.h"

#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

namespace Menge
{
    static const int INVALID_HANDLE_VALUE = -1;

	//////////////////////////////////////////////////////////////////////////
	PosixInputStream::PosixInputStream()
		: m_hFile(INVALID_HANDLE_VALUE)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	PosixInputStream::~PosixInputStream()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool PosixInputStream::open( const String& _filename )
	{
		m_hFile = ::open( _filename.c_str(), O_RDONLY );

		if( m_hFile == INVALID_HANDLE_VALUE )
		{
			return false;
		}

		struct stat hStat;
		std::fill( reinterpret_cast<unsigned char*>( &hStat ),
			reinterpret_cast<unsigned char*>( &hStat ) + sizeof( hStat ), 0 );

		if( fstat( m_hFile, &hStat ) != 0 )
		{
			return false;
		}
		
		m_size = hStat.st_size;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void PosixInputStream::destroy()
	{
		if( m_hFile != INVALID_HANDLE_VALUE )
		{
			::close( m_hFile );

			m_hFile = INVALID_HANDLE_VALUE;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	int PosixInputStream::read( void* _buf, int _count )
	{
		ssize_t bytesRead = 0;
		ssize_t result = ::read( m_hFile, _buf, _count );

		if( result == -1 )
		{
			return 0;
		}

		return static_cast<int>( bytesRead );
	}
	//////////////////////////////////////////////////////////////////////////
	void PosixInputStream::seek( int _pos )
	{
		::lseek( m_hFile, _pos, SEEK_SET );
	}
    //////////////////////////////////////////////////////////////////////////
    int PosixInputStream::tell() const
    {
        return lseek( m_hFile, 0, SEEK_CUR );
    }
	//////////////////////////////////////////////////////////////////////////
	int PosixInputStream::size() const
	{
		return static_cast<int>( m_size );
	}
	//////////////////////////////////////////////////////////////////////////
    bool PosixInputStream::time( time_t & _time ) const
    {
        struct stat buffer;
        if( fstat( m_hFile, &buffer ) )
            return false;
        _time = buffer.st_mtime; // last modification?
        return true;
    }
}	// namespace Menge
