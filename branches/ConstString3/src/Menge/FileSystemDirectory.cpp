/*
 *	FileSystemDirectory.cpp
 *
 *	Created by _Berserk_ on 2.9.2009
 *	Copyright 2009 Menge. All rights reserved.
 *
 */

#	include "FileSystemDirectory.h"

#	include "FileBufferProvider.h"

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
		, m_fileBufferProvider(NULL)
	{

	}
	//////////////////////////////////////////////////////////////////////////
	FileSystemDirectory::~FileSystemDirectory()
	{
		delete m_fileBufferProvider;
	}
	//////////////////////////////////////////////////////////////////////////
	bool FileSystemDirectory::initialize( const WString& _path, FileEngine * _fileEngine, bool _create )
	{
		m_interface = FileEngine::get()
			->getFileSystemInterface();

		if( m_interface == NULL )
		{
			MENGE_LOG_ERROR( "Error: (FileSystemDirectory::initialize) can't get FileSystem interface" );
            
			return false;
		}

		m_fileEngine = _fileEngine;

		m_path = _path;
//        Utils::collapsePath( path, m_path );
        
		if( m_interface->existFile( m_path ) == false )
		{
			if( _create == false )
			{
				MENGE_LOG_ERROR( "FileSystemDirectory::initialize failed to open directory %S"
					, m_path.c_str() 
					);
                
				return false;
			}

			MENGE_LOG_WARNING( "FileSystemDirectory::initialize create directory %S"
				, m_path.c_str() 
				);

			if( m_interface->createFolder( m_path ) == false )
			{
				MENGE_LOG_ERROR( "FileSystemDirectory::initialize failed to create directory %S"
					, m_path.c_str() 
					);

				return true;
			}
		}

		m_fileBufferProvider = new FileBufferProvider();
		
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	const WString & FileSystemDirectory::getPath() const
	{
		return m_path;
	}
	//////////////////////////////////////////////////////////////////////////
	bool FileSystemDirectory::existFile( const WString& _filename )
	{
		this->makeFullname_( _filename, m_fullnameCache );

		return m_interface->existFile( m_fullnameCache );
	}
	//////////////////////////////////////////////////////////////////////////
	FileBufferProvider * FileSystemDirectory::getBufferProvider() const
	{
		return m_fileBufferProvider;
	}
	//////////////////////////////////////////////////////////////////////////
	FileInputStreamInterface* FileSystemDirectory::createInputFile()
	{
		//BufferedFileInput* bufferedFi = m_fileInputPool.get();
		FileInputStreamInterface * inputStream = m_interface->createInputStream();
				
		//bufferedFi->setFileSystem( this );

		return inputStream;
	}
	//////////////////////////////////////////////////////////////////////////
	bool FileSystemDirectory::openInputFile( const WString& _filename, FileInputStreamInterface* _file )
	{
		if( _file == NULL )
		{
			MENGE_LOG_ERROR( "FileSystemDirectory::openInputFile failed _file == NULL"
				);

			return false;
		}

		this->makeFullname_( _filename, m_fullnameCache );

		if( _file->open( m_fullnameCache ) == false )
        {
            return false;
        }
		//FileInputStreamInterface* fi = m_interface->openInputStream( m_fullnameCache );

		//BufferedFileInput* bufferedFi = 
			//static_cast<BufferedFileInput*>( _file );

		//bufferedFi->loadStream( fi );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void FileSystemDirectory::closeInputFile( FileInputStreamInterface* _inputFile )
	{
		if( _inputFile == NULL )
		{
			return;
		}

		//BufferedFileInput* bufferedFi = 
			//static_cast<BufferedFileInput*>(_inputFile);

		//FileInputStreamInterface* fi = bufferedFi->unloadStream();

		
		m_interface->closeInputStream( _inputFile );

		//delete bufferedFi;
		//m_fileInputPool.release( bufferedFi );
	}
	//////////////////////////////////////////////////////////////////////////
	FileOutputStreamInterface* FileSystemDirectory::createOutputFile()
	{
		//SimpleFileOutput* fileOutput = m_fileOutputPool.get();
		SimpleFileOutput* fileOutput = new SimpleFileOutput;

		fileOutput->setFileSystem( this );

		return fileOutput;
	}
	//////////////////////////////////////////////////////////////////////////	
	bool FileSystemDirectory::openOutputFile( const WString& _filename, FileOutputStreamInterface* _file )
	{
		this->makeFullname_( _filename, m_fullnameCache );
		
		FileOutputStreamInterface* fo = m_interface->openOutputStream( m_fullnameCache );
		if( fo == NULL )
		{
			MENGE_LOG_ERROR( "Error: (FileSystemDirectory::openOutputFile) failed to open output stream '%S'"
				, m_fullnameCache.c_str() 
				);

			return false;
		}

		SimpleFileOutput* fileOutput = 
			static_cast<SimpleFileOutput*>( _file );

		fileOutput->loadStream( fo );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void FileSystemDirectory::closeOutputFile( FileOutputStreamInterface* _outputFile )
	{
		if( _outputFile == NULL )
		{
			return;
		}

		SimpleFileOutput* fileOutput = 
			static_cast<SimpleFileOutput*>( _outputFile );

		FileOutputStreamInterface* fo = fileOutput->unloadStream();
		m_interface->closeOutputStream( fo );

		//m_fileOutputPool.release( fileOutput );
		delete fileOutput;
	}
	//////////////////////////////////////////////////////////////////////////
	bool FileSystemDirectory::createDirectory( const WString& _path )
	{
		makeFullname_( _path, m_fullnameCache );
	
		return m_interface->createFolder( m_fullnameCache );
	}
	//////////////////////////////////////////////////////////////////////////
	void FileSystemDirectory::removeDirectory( const WString& _path )
	{
		makeFullname_( _path, m_fullnameCache );

		m_interface->deleteFolder( m_fullnameCache );
	}
	//////////////////////////////////////////////////////////////////////////
	void FileSystemDirectory::removeFile( const WString& _filename )
	{
		makeFullname_( _filename, m_fullnameCache );

		m_interface->deleteFile( m_fullnameCache );
	}
	//////////////////////////////////////////////////////////////////////////
	void FileSystemDirectory::makeFullname_( const WString& _path, WString& _fullname )
	{
		_fullname = m_path;
		_fullname += _path;

		//Utils::collapsePath( _fullname, _fullname );
	}
	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge
