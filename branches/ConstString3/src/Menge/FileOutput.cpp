/*
 *	FileOutput.cpp
 *
 *	Created by _Berserk_ on 7.9.2009
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
		, m_opened( false )
	{

	}
	//////////////////////////////////////////////////////////////////////////
	FileOutput::~FileOutput()
	{
		assert( m_opened == false );
	}
	//////////////////////////////////////////////////////////////////////////
	void FileOutput::setFileSystem( FileSystem* _fileSystem )
	{
		assert( m_opened == false );
		m_fileSystem = _fileSystem;
	}
	//////////////////////////////////////////////////////////////////////////
	FileSystem* FileOutput::getFileSystem()
	{
		return m_fileSystem;
	}
	//////////////////////////////////////////////////////////////////////////
	bool FileOutput::open( const String& _filename )
	{
		assert( m_opened == false );

		if( m_fileSystem == NULL )
		{
			return false;
		}

		m_opened = m_fileSystem->openOutputFile( _filename, this );

		return m_opened;
	}
	//////////////////////////////////////////////////////////////////////////
	void FileOutput::close()
	{
		m_opened = false;

		if( m_fileSystem == NULL )
		{
			return;
		}

		m_fileSystem->closeOutputFile( this );
	}
	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge
