/*
 *	PosixInputStream.cpp
 *
 *	Created by _Berserk_ on 7.10.2009
 *	Copyright 2009 Menge. All rights reserved.
 *
 */

#	include "PosixInputStream.h"
#   include "PosixFileSystem.h"

#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	PosixInputStream::PosixInputStream()
		: m_hFile( -1 )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	PosixInputStream::~PosixInputStream()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool PosixInputStream::open( const String& _filename )
	{
        m_hFile = ::open( ( String( g_resDirectory ) + _filename ).c_str(), O_RDONLY );
        
        MAGIC_LOG( "trying to open \"%s\", result %i\n", ( String( g_resDirectory ) + _filename ).c_str(), m_hFile );
        
        if( m_hFile == -1 )
        {
            m_hFile = ::open( _filename.c_str(), O_RDONLY );
            
            MAGIC_LOG( "trying to open \"%s\", result %i\n"
                      , _filename.c_str()
                      , m_hFile 
                      );
            
            if( m_hFile == -1 )
            {
                return false;
            }
		}

		struct stat hStat;
		std::fill( reinterpret_cast<unsigned char*>( &hStat ),
			reinterpret_cast<unsigned char*>( &hStat ) + sizeof( hStat ), 0 );
		if( fstat( m_hFile, &hStat ) != 0 )
		{
            MAGIC_LOG( "std::fill failed, errno = %i\n", errno );
			return false;
		}
		
		m_size = hStat.st_size;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void PosixInputStream::close()
	{
		if( m_hFile != -1 )
		{
			::close( m_hFile );
			m_hFile = -1;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	int PosixInputStream::read( void* _buf, int _count )
	{
		ssize_t bytesRead = 0;
		ssize_t result = ::read( m_hFile, _buf, _count );
		if( result != -1 )
		{
			bytesRead = result;
		}
		return static_cast<int>( bytesRead );
	}
	//////////////////////////////////////////////////////////////////////////
	void PosixInputStream::seek( int _pos )
	{
		::lseek( m_hFile, _pos, SEEK_SET );
	}
	//////////////////////////////////////////////////////////////////////////
	int PosixInputStream::size() const
	{
		return static_cast<int>( m_size );
	}
	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge
