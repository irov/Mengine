/*
 *	FileSystemDirectory.cpp
 *
 *	Created by _Berserk_ on 2.9.2009
 *	Copyright 2009 Menge. All rights reserved.
 *
 */

#	include "FileSystemDirectory.h"

#	include <cassert>

#	include "BufferedFileInput.h"
#	include "FileEngine.h"
#	include "LogEngine.h"
#	include "SimpleFileOutput.h"
#	include "Utils.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	FileSystemDirectory::FileSystemDirectory()
		: m_interface( NULL )
	{

	}
	//////////////////////////////////////////////////////////////////////////
	FileSystemDirectory::~FileSystemDirectory()
	{

	}
	//////////////////////////////////////////////////////////////////////////
	bool FileSystemDirectory::initialize( const String& _path, bool _create )
	{
		m_interface = FileEngine::hostage()
							->getFileSystemInterface();

		if( m_interface == NULL )
		{
			MENGE_LOG_ERROR( "Error: (FileSystemDirectory::initialize) can't get FileSystem interface" );
			return false;
		}

		if( existFile( _path ) == false )
		{
			if( _create == false || createDirectory( _path ) == false )
			{
				return false;
			}
		}

		m_path = _path;
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool FileSystemDirectory::existFile( const String& _filename )
	{
		String fullname;
		makeFullname_( _filename, &fullname );
		return m_interface->existFile( fullname );
	}
	//////////////////////////////////////////////////////////////////////////
	FileInput* FileSystemDirectory::createInputFile()
	{
		BufferedFileInput* bufferedFi = m_fileInputPool.get();
		bufferedFi->setFileSystem( this );
		return bufferedFi;
	}
	//////////////////////////////////////////////////////////////////////////
	bool FileSystemDirectory::openInputFile( const String& _filename, FileInput* _file )
	{
		String fullname;
		makeFullname_( _filename, &fullname );

		InputStreamInterface* fi = m_interface->openInputStream( fullname );
		if( fi == NULL )
		{
			MENGE_LOG_ERROR( "Error: (FileSystemDirectory::openInputFile) failed to open input stream '%s'"
				, _filename.c_str() 
				);

			return false;
		}
		BufferedFileInput* bufferedFi = static_cast<BufferedFileInput*>( _file );
		bufferedFi->loadStream( fi );
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void FileSystemDirectory::closeInputFile( FileInput* _inputFile )
	{
		BufferedFileInput* bufferedFi = static_cast< BufferedFileInput* >( _inputFile );
		assert( bufferedFi != NULL );
		InputStreamInterface* fi = bufferedFi->unloadStream();
		m_interface->closeInputStream( fi );
		m_fileInputPool.release( bufferedFi );
	}
	//////////////////////////////////////////////////////////////////////////
	FileOutput* FileSystemDirectory::createOutputFile()
	{
		SimpleFileOutput* fileOutput = m_fileOutputPool.get();
		fileOutput->setFileSystem( this );
		return fileOutput;
	}
	//////////////////////////////////////////////////////////////////////////	
	bool FileSystemDirectory::openOutputFile( const String& _filename, FileOutput* _file )
	{
		String fullname;
		makeFullname_( _filename, &fullname );
		
		OutputStreamInterface* fo = m_interface->openOutputStream( fullname );
		if( fo == NULL )
		{
			MENGE_LOG_ERROR( "Error: (FileSystemDirectory::openOutputFile) failed to open output stream '%s'"
				, _filename.c_str() 
				);

			return false;
		}

		SimpleFileOutput* fileOutput = static_cast<SimpleFileOutput*>( _file );
		fileOutput->loadStream( fo );
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void FileSystemDirectory::closeOutputFile( FileOutput* _outputFile )
	{
		SimpleFileOutput* fileOutput = static_cast< SimpleFileOutput* >( _outputFile );
		assert( fileOutput != NULL );
		OutputStreamInterface* fo = fileOutput->unloadStream();
		m_interface->closeOutputStream( fo );
		m_fileOutputPool.release( fileOutput );
	}
	//////////////////////////////////////////////////////////////////////////
	bool FileSystemDirectory::createDirectory( const String& _path )
	{
		String fullname;
		makeFullname_( _path, &fullname );
	
		return m_interface->createFolder( fullname );
	}
	//////////////////////////////////////////////////////////////////////////
	void FileSystemDirectory::removeDirectory( const String& _path )
	{
		String fullname;
		makeFullname_( _path, &fullname );

		m_interface->deleteFolder( fullname );
	}
	//////////////////////////////////////////////////////////////////////////
	void FileSystemDirectory::removeFile( const String& _filename )
	{
		String fullname;
		makeFullname_( _filename, &fullname );

		m_interface->deleteFile( fullname );
	}
	//////////////////////////////////////////////////////////////////////////
	void FileSystemDirectory::makeFullname_( const String& _path, String* _fullname )
	{
		assert( _fullname != NULL );
		if( m_path.empty() == false )
		{
			*_fullname = m_path;
			(*_fullname) += "/";
			(*_fullname) += _path;
		}
		else
		{
			(*_fullname) = _path;
		}
		Utils::collapsePath( (*_fullname), _fullname );
	}
	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge
