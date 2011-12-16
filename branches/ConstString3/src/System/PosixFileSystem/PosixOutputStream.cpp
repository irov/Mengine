/*
 *	PosixOutputStream.cpp
 *
 *	Created by _Berserk_ on 7.10.2009
 *	Copyright 2009 Menge. All rights reserved.
 *
 */

#	include "PosixOutputStream.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	PosixOutputStream::PosixOutputStream()
		: m_hFile( NULL )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	PosixOutputStream::~PosixOutputStream()
	{
		if( m_hFile != NULL )
		{
			fclose( m_hFile );
			m_hFile = NULL;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool PosixOutputStream::open( const String& _filename )
	{
		m_hFile = fopen( _filename.c_str(), "wb" );
		if( m_hFile == NULL )
		{
			return false;
		}
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void PosixOutputStream::write( const void * _data, int _count )
	{
		fwrite( _data, _count, 1, m_hFile );
	}
	//////////////////////////////////////////////////////////////////////////
	void PosixOutputStream::flush()
	{
		fflush( m_hFile );
	}
	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge
