/*
 *	FileOutput.cpp
 *
 *	Created by _Berserk_ on 7.9.2009
 *	Copyright 2009 Menge. All rights reserved.
 *
 */

#	include "FileOutput.h"

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
	}
	//////////////////////////////////////////////////////////////////////////
	void FileOutput::setFileSystem( FileSystem* _fileSystem )
	{
		m_fileSystem = _fileSystem;
	}
	//////////////////////////////////////////////////////////////////////////
	FileSystem* FileOutput::getFileSystem()
	{
		return m_fileSystem;
	}
	//////////////////////////////////////////////////////////////////////////
	bool FileOutput::open( const WString& _filename )
	{
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
