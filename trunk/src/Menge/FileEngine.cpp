#	include "FileEngine.h"

#	include <vector>
#	include <algorithm>
#	include <cassert>

#	include "Logger/Logger.h"
#	include "FileSystem.h"

#	include "Factory/FactoryDefault.h"
#	include "FileSystemDirectory.h"
#	include "FileSystemZip.h"
#	include "FileInput.h"
#	include "FileOutput.h"

#	include "Core/String.h"
#	include "Core/File.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	FileEngine::FileEngine()
		: m_interface( NULL )
		, m_baseDir( "." )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	FileEngine::~FileEngine()
	{
		for( TFileSystemMap::iterator it = m_fileSystemMap.begin(), it_end = m_fileSystemMap.end();
			it != it_end;
			++it )
		{
			it->second->destroy();
		}

		if( m_fileSystemMemoryMapped )
		{
			m_fileSystemMemoryMapped->destroy();
		}
		
		if( m_interface != NULL )
		{
			releaseInterfaceSystem( m_interface );
			m_interface = NULL;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool FileEngine::initialize()
	{
		FactoryManager::registerFactory( "", new FactoryDefault<FileSystemDirectory>() );
		FactoryManager::registerFactory( "pak", new FactoryDefault<FileSystemZip>() );
		FactoryManager::registerFactory( "zip", new FactoryDefault<FileSystemZip>() );
		FactoryManager::registerFactory( "memory", new FactoryDefault<FileSystemMemoryMapped>() );

		m_fileSystemMemoryMapped = FactoryManager::createObjectT<FileSystemMemoryMapped>( "memory" );

		if( m_fileSystemMemoryMapped->initialize( "", false ) == false )
		{
			return false;
		}

		bool result = initInterfaceSystem( &m_interface );
		if( ( result == false )|| ( m_interface == NULL ) )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool FileEngine::mountFileSystem( const String& _fileSystemName, const String& _path, bool _create )
	{
		TFileSystemMap::iterator it_find = m_fileSystemMap.find( _fileSystemName );
		if( it_find != m_fileSystemMap.end() )
		{
			MENGE_LOG( "Warning: (FileEngine::mountFileSystem) FileSystem with name '%s' is already mount\n"
				"Remount would be performed"
				, _fileSystemName.c_str() 
				);

			unmountFileSystem( _fileSystemName );
		}

		String finalPath = _path;
		String::size_type seppos = finalPath.find_last_of( '/' );
		if( seppos != String::npos )
		{
			finalPath.erase( 0, seppos+1 );
		}

		String typeExt;
		Utils::getFileExt( typeExt, finalPath );
		//printf( "typeExt %s", typeExt.c_str() )

		FileSystem * fs = FactoryManager::createObjectT<FileSystem>( typeExt );

		if( fs == NULL )
		{
			//MENGE_LOG_ERROR( "Error: (FileEngine::mountFileSystem) can't find FileSystem for object '%s'"
			//	, _path.c_str() 
			//	);

			//return false;

			// try mount as Directory
			fs = FactoryManager::createObjectT<FileSystem>( "" );
		}

		String fullpath = _path;

		if( s_isAbsolutePath( _path ) == false )
		{
			fullpath = m_baseDir + "/" + fullpath;
		}

		if( fs->initialize( fullpath, _create ) == false )
		{
			MENGE_LOG_ERROR( "Error: (FileEngine::mountFileSystem) can't initialize FileSystem for object '%s'"
				, _path.c_str() 
				);

			fs->destroy();
			return false;
		}

		m_fileSystemMap.insert( std::make_pair( _fileSystemName, fs ) );
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void FileEngine::unmountFileSystem( const String& _fileSystemName )
	{
		TFileSystemMap::iterator it_find = m_fileSystemMap.find( _fileSystemName );
		if( it_find == m_fileSystemMap.end() )
		{
			MENGE_LOG_ERROR( "Error: (FileEngine::unmountFileSystem) FileSystem with name '%s' not mount"
				, _fileSystemName.c_str() 
				);

			return;
		}

		it_find->second->destroy();
		m_fileSystemMap.erase( it_find );
	}
	//////////////////////////////////////////////////////////////////////////
	bool FileEngine::existFile( const String& _fileSystemName, const String& _filename )
	{
		TFileSystemMap::iterator it_find = m_fileSystemMap.find( _fileSystemName );
		if( it_find != m_fileSystemMap.end() )
		{
			return it_find->second->existFile( _filename );
		}

		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	FileInputInterface* FileEngine::createFileInput( const String& _fileSystemName )
	{
		TFileSystemMap::iterator it_find = m_fileSystemMap.find( _fileSystemName );
		if( it_find == m_fileSystemMap.end() )
		{
			MENGE_LOG_ERROR( "Error: (FileEngine::createFileInput) FileSystem '%s' not mount"
				, _fileSystemName.c_str()
				);

			return NULL;
		}

		FileInputInterface* file = it_find->second->createInputFile();

		return file;
	}
	//////////////////////////////////////////////////////////////////////////
	FileInputInterface* FileEngine::openFileInput( const String& _fileSystemName, const String& _filename )
	{
		FileInputInterface * file = 0;

		if( ( _fileSystemName == "" ) && m_fileSystemMemoryMapped->existFile( _filename ) == true )
		{
			file = m_fileSystemMemoryMapped->createInputFile();

			if( m_fileSystemMemoryMapped->openInputFile( _filename, file ) == false )
			{
				MENGE_LOG_ERROR( "Warning: (FileEngine::openFileInput) troubles while opening mapped file '%s'"
					, _filename.c_str() 
					);

				m_fileSystemMemoryMapped->closeInputFile( file );
				file = NULL;
			}
		}
		else
		{
			file = this->createFileInput( _fileSystemName );

			if( file == 0 )
			{
				return 0;
			}

			if( file->open( _filename ) == false )
			{
				closeFileInput( file );
				file = NULL;
			}		
		}

		return file;

		//TFileSystemMap::iterator it_find = m_fileSystemMap.find( _fileSystemName );
		//if( it_find == m_fileSystemMap.end() )
		//{
		//	MENGE_LOG_ERROR( "Error: (FileEngine::openFileInput) FileSystem '%s' not mount"
		//		, _fileSystemName.c_str() 
		//		);

		//	return NULL;
		//}

		//MENGE_LOG( "-- Open File %s", _filename.c_str() );
		//FileInput* file;
		//// check if file already mapped
		//if( ( _fileSystemName == "" ) && m_fileSystemMemoryMapped->existFile( _filename ) == true )
		//{
		//	file = m_fileSystemMemoryMapped->createInputFile();
		//	if( m_fileSystemMemoryMapped->openInputFile( _filename, file ) == true )
		//	{
		//		return file;
		//	}
		//	else
		//	{
		//		MENGE_LOG_ERROR( "Warning: (FileEngine::openFileInput) troubles while opening mapped file '%s'"
		//			, _filename.c_str() 
		//			);

		//		m_fileSystemMemoryMapped->closeInputFile( file );
		//	}
		//	
		//}
		////if( it_find->second->existFile( _filename ) == false )
		////{
		////	MENGE_LOG_ERROR( "Error: (FileEngine::openFileInput) file not found '%s': '%s'",
		////		_fileSystemName.c_str(), _filename.c_str() );
		////	return NULL;
		////}

		//file = it_find->second->createInputFile();
		//if( file->open( _filename ) == false )
		//{
		//	closeFileInput( file );
		//	file = NULL;
		//}
		////FileInputInterface* file = it_find->second->openInputFile( _filename );


		//return file;
	}
	//////////////////////////////////////////////////////////////////////////
	void FileEngine::closeFileInput( FileInputInterface* _file )
	{
		_file->close();
	}
	//////////////////////////////////////////////////////////////////////////
	FileInputInterface * FileEngine::openMappedFile( const String& _filename )
	{
		assert( m_interface != NULL );

		/*String fullpath = _filename;

		if( s_isAbsolutePath( _filename ) == false )
		{
			fullpath = m_baseDir + "/" + fullpath;
		}*/
		FileInputInterface* file = m_fileSystemMemoryMapped->createInputFile();
		if( m_fileSystemMemoryMapped->openInputFile( _filename, file ) == false )
		{
			MENGE_LOG_ERROR( "Error: (FileEngine::openMappedFile) can't open file '%s'"
				, _filename.c_str()
				);

			m_fileSystemMemoryMapped->closeInputFile( file );
			file = NULL;
		}

		return file;
	}
	//////////////////////////////////////////////////////////////////////////
	void FileEngine::closeMappedFile( FileInputInterface* _file )
	{
		assert( _file != NULL );
		_file->close();
	}
	//////////////////////////////////////////////////////////////////////////
	FileOutputInterface * FileEngine::createFileOutput( const String& _fileSystemName )
	{
		TFileSystemMap::iterator it_find = m_fileSystemMap.find( _fileSystemName );
		if( it_find == m_fileSystemMap.end() )
		{
			MENGE_LOG_ERROR( "Error: (FileEngine::createFileInput) FileSystem '%s' not mount"
				, _fileSystemName.c_str()
				);

			return NULL;
		}

		FileOutputInterface * file = it_find->second->createOutputFile();

		return file;
	}
	//////////////////////////////////////////////////////////////////////////
	FileOutputInterface* FileEngine::openFileOutput( const String& _fileSystemName, const String& _filename )
	{
		FileOutputInterface * file = this->createFileOutput( _fileSystemName );

		if( file == 0 )
		{
			return 0;
		}

		if( file->open( _filename ) == false )
		{
			closeFileOutput( file );
			file = NULL;
		}

		return file;
	}
	//////////////////////////////////////////////////////////////////////////
	void FileEngine::closeFileOutput( FileOutputInterface* _file )
	{
		assert( _file != NULL );
		_file->close();
	}
	//////////////////////////////////////////////////////////////////////////
	void FileEngine::setBaseDir( const String& _baseDir )
	{
		if( _baseDir.empty() == true )	// current dir
		{
			m_baseDir = ".";
		}
		else
		{
			m_baseDir = _baseDir;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	const String& FileEngine::getBaseDir() const
	{
		return m_baseDir;
	}
	//////////////////////////////////////////////////////////////////////////
	bool FileEngine::createDirectory( const String& _fileSystemName, const String& _path )
	{
		TFileSystemMap::iterator it_find = m_fileSystemMap.find( _fileSystemName );
		if( it_find == m_fileSystemMap.end() )
		{
			MENGE_LOG_ERROR( "Error: (FileEngine::createDirectory) FileSystem '%s' not mount"
				, _fileSystemName.c_str() 
				);

			return false;
		}

		FileSystem* fs = it_find->second;

		String::size_type idx = 0;
		idx = _path.find( '/', idx );
		while( idx != String::npos )
		{
			String subDir = _path.substr( 0, idx );
			if( fs->existFile( subDir ) == false &&
				fs->createDirectory( subDir ) == false )
			{
				return false;
			}
			idx = _path.find( '/', idx+1 );
		}
		if( fs->createDirectory( _path ) == false )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void FileEngine::removeDirectory( const String& _fileSystemName, const String& _path )
	{
		TFileSystemMap::iterator it_find = m_fileSystemMap.find( _fileSystemName );
		if( it_find == m_fileSystemMap.end() )
		{
			MENGE_LOG_ERROR( "Error: (FileEngine::removeDirectory) FileSystem '%s' not mount"
				, _fileSystemName.c_str() 
				);

			return;
		}

		it_find->second->removeDirectory( _path );
	}
	//////////////////////////////////////////////////////////////////////////
	void FileEngine::removeFile( const String& _fileSystemName, const String& _filename )
	{
		TFileSystemMap::iterator it_find = m_fileSystemMap.find( _fileSystemName );
		if( it_find == m_fileSystemMap.end() )
		{
			MENGE_LOG_ERROR( "Error: (FileEngine::removeFile) FileSystem '%s' not mount"
				, _fileSystemName.c_str() 
				);

			return;
		}
		
		it_find->second->removeFile( _filename );
	}
	//////////////////////////////////////////////////////////////////////////
	FileSystemInterface* FileEngine::getFileSystemInterface()
	{
		return m_interface;
	}
	//////////////////////////////////////////////////////////////////////////
	bool FileEngine::s_isAbsolutePath( const String& _path )
	{
		if( ( _path.size() > 1 && _path[0] == '/' ) 
			|| ( _path.size() > 2 && _path[1] == ':' ) )
		{
			return true;
		}
		return false;
	}
	//////////////////////////////////////////////////////////////////////////
}
