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

#	include "Core/String.h"
#	include "Core/File.h"

#	include "Consts.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	FileSystemDirectory::FileSystemDirectory()
		: m_interface(NULL)
		, m_fileEngine(NULL)
	{

	}
	//////////////////////////////////////////////////////////////////////////
	FileSystemDirectory::~FileSystemDirectory()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool FileSystemDirectory::initialize( const String& _path, FileEngine * _fileEngine, bool _create )
	{
		m_interface = FileEngine::get()
			->getFileSystemInterface();

		if( m_interface == NULL )
		{
			MENGE_LOG_ERROR( "Error: (FileSystemDirectory::initialize) can't get FileSystem interface" );
			return false;
		}

		m_fileEngine = _fileEngine;

		if( existFile( _path ) == false )
		{
			if( _create == false || m_fileEngine->createDirectory( Consts::get()->c_builtin_empty, _path ) == false )
			{
				MENGE_LOG_ERROR( "Failed to create directory %s", _path.c_str() );
				return false;
			}
		}

		m_path = _path;

		if( _path.empty() == false )
		{
			m_path += "/";
		}
		//Utils::collapsePath( _path, m_path );
		
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	const String & FileSystemDirectory::getPath() const
	{
		return m_path;
	}
	//////////////////////////////////////////////////////////////////////////
	bool FileSystemDirectory::existFile( const String& _filename )
	{
		String fullname;
		makeFullname_( _filename, fullname );
		return m_interface->existFile( fullname );
	}
	//////////////////////////////////////////////////////////////////////////
	FileInputInterface* FileSystemDirectory::createInputFile()
	{
		//BufferedFileInput* bufferedFi = m_fileInputPool.get();
		BufferedFileInput* bufferedFi = new BufferedFileInput;
		bufferedFi->setFileSystem( this );
		return bufferedFi;
	}
	//////////////////////////////////////////////////////////////////////////
	bool FileSystemDirectory::openInputFile( const String& _filename, FileInputInterface* _file )
	{
		String fullname;
		makeFullname_( _filename, fullname );

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
	void FileSystemDirectory::closeInputFile( FileInputInterface* _inputFile )
	{
		if( _inputFile == NULL )
		{
			return;
		}

		BufferedFileInput* bufferedFi = 
			static_cast<BufferedFileInput*>(_inputFile);

		InputStreamInterface* fi = bufferedFi->unloadStream();
		m_interface->closeInputStream( fi );

		delete bufferedFi;
		//m_fileInputPool.release( bufferedFi );
	}
	//////////////////////////////////////////////////////////////////////////
	FileOutputInterface* FileSystemDirectory::createOutputFile()
	{
		//SimpleFileOutput* fileOutput = m_fileOutputPool.get();
		SimpleFileOutput* fileOutput = new SimpleFileOutput;
		fileOutput->setFileSystem( this );
		return fileOutput;
	}
	//////////////////////////////////////////////////////////////////////////	
	bool FileSystemDirectory::openOutputFile( const String& _filename, FileOutputInterface* _file )
	{
		String fullname;
		makeFullname_( _filename, fullname );
		
		OutputStreamInterface* fo = m_interface->openOutputStream( fullname );
		if( fo == NULL )
		{
			MENGE_LOG_ERROR( "Error: (FileSystemDirectory::openOutputFile) failed to open output stream '%s'"
				, _filename.c_str() 
				);

			return false;
		}

		SimpleFileOutput* fileOutput = 
			static_cast<SimpleFileOutput*>( _file );

		fileOutput->loadStream( fo );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void FileSystemDirectory::closeOutputFile( FileOutputInterface* _outputFile )
	{
		if( _outputFile == NULL )
		{
			return;
		}

		SimpleFileOutput* fileOutput = 
			static_cast<SimpleFileOutput*>( _outputFile );

		OutputStreamInterface* fo = fileOutput->unloadStream();
		m_interface->closeOutputStream( fo );

		//m_fileOutputPool.release( fileOutput );
		delete fileOutput;
	}
	//////////////////////////////////////////////////////////////////////////
	bool FileSystemDirectory::createDirectory( const String& _path )
	{
		String fullname;
		makeFullname_( _path, fullname );
	
		return m_interface->createFolder( fullname );
	}
	//////////////////////////////////////////////////////////////////////////
	void FileSystemDirectory::removeDirectory( const String& _path )
	{
		String fullname;
		makeFullname_( _path, fullname );

		m_interface->deleteFolder( fullname );
	}
	//////////////////////////////////////////////////////////////////////////
	void FileSystemDirectory::removeFile( const String& _filename )
	{
		String fullname;
		makeFullname_( _filename, fullname );

		m_interface->deleteFile( fullname );
	}
	//////////////////////////////////////////////////////////////////////////
	void FileSystemDirectory::makeFullname_( const String& _path, String& _fullname )
	{
		_fullname = m_path;
		_fullname += _path;

		Utils::collapsePath( _fullname, _fullname );
	}
	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge
