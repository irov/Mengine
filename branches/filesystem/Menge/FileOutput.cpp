/*
 *	FileOutput.cpp
 *
 *	Created by _Berserk_ on 2.9.2009
 *	Copyright 2009 Menge. All rights reserved.
 *
 */

#	include "FileOutput.h"

#	include <cassert>

#	include "FileSystem.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	FileOutput::FileOutput()
		: m_fileSystem( NULL )
		, m_iStream( NULL )
		, m_iStreamCursor( 0 )
	{

	}
	//////////////////////////////////////////////////////////////////////////
	FileOutput::~FileOutput()
	{

	}
	//////////////////////////////////////////////////////////////////////////
	void FileOutput::loadStream( FileSystem* _fileSystem, OutputStreamInterface* _iStream )
	{
		m_fileSystem = _fileSystem;
		m_iStream = _iStream;
		assert( m_fileSystem != NULL );
		assert( m_iStream != NULL );
		m_iStreamCursor = 0;
	}
	//////////////////////////////////////////////////////////////////////////
	OutputStreamInterface* FileOutput::unloadStream()
	{
		OutputStreamInterface* stream = m_iStream;
		m_fileSystem = NULL;
		m_iStream = NULL;
		return stream;
	}
	//////////////////////////////////////////////////////////////////////////
	void FileOutput::write( const void* _data, int _count )
	{
		m_iStream->write( _data, _count );
		m_iStreamCursor += _count;
	}
	//////////////////////////////////////////////////////////////////////////
	void FileOutput::flush()
	{
		m_iStream->flush();
	}
	//////////////////////////////////////////////////////////////////////////
	void FileOutput::close()
	{
		m_fileSystem->closeOutputFile( this );
	}
	//////////////////////////////////////////////////////////////////////////
	int FileOutput::tell()
	{
		return m_iStreamCursor;
	}
	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge
