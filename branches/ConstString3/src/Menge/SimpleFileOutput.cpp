/*
 *	FileOutput.cpp
 *
 *	Created by _Berserk_ on 2.9.2009
 *	Copyright 2009 Menge. All rights reserved.
 *
 */

#	include "SimpleFileOutput.h"

#	include <cassert>

#	include "FileSystem.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	SimpleFileOutput::SimpleFileOutput()
		: m_iStream( NULL )
		, m_iStreamCursor( 0 )
	{

	}
	//////////////////////////////////////////////////////////////////////////
	SimpleFileOutput::~SimpleFileOutput()
	{

	}
	//////////////////////////////////////////////////////////////////////////
	void SimpleFileOutput::loadStream( FileOutputStreamInterface* _iStream )
	{
		m_iStream = _iStream;
		assert( m_iStream != NULL );
		m_iStreamCursor = 0;
	}
	//////////////////////////////////////////////////////////////////////////
	FileOutputStreamInterface* SimpleFileOutput::unloadStream()
	{
		FileOutputStreamInterface* stream = m_iStream;
		m_iStream = NULL;
		return stream;
	}
	//////////////////////////////////////////////////////////////////////////
	void SimpleFileOutput::write( const void* _data, int _count )
	{
		m_iStream->write( _data, _count );
		m_iStreamCursor += _count;
	}
	//////////////////////////////////////////////////////////////////////////
	void SimpleFileOutput::flush()
	{
		m_iStream->flush();
	}
	//////////////////////////////////////////////////////////////////////////
	int SimpleFileOutput::tell()
	{
		return m_iStreamCursor;
	}
	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge
