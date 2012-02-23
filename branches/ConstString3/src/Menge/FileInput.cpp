/*
 *	FileInput.cpp
 *
 *	Created by _Berserk_ on 7.9.2009
 *	Copyright 2009 Menge. All rights reserved.
 *
 */

#	include "FileInput.h"

#	include "FileSystem.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	FileInput::FileInput()
		: m_fileSystem( NULL )
		, m_opened( false )
	{

	}
	//////////////////////////////////////////////////////////////////////////
	FileInput::~FileInput()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void FileInput::setFileSystem( FileSystem* _fileSystem )
	{
		m_fileSystem = _fileSystem;
	}
	//////////////////////////////////////////////////////////////////////////
	FileSystem* FileInput::getFileSystem()
	{
		return m_fileSystem;
	}
	//////////////////////////////////////////////////////////////////////////
	bool FileInput::open( const String& _filename )
	{
		if( m_fileSystem == NULL )
		{
			return false;
		}

		m_opened = m_fileSystem->openInputFile( _filename, this );

		return m_opened;
	}
	//////////////////////////////////////////////////////////////////////////
	void FileInput::close()
	{
		m_opened = false;

		if( m_fileSystem == NULL )
		{
			return;
		}

		m_fileSystem->closeInputFile( this );
	}
	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge
