#	include "FileEngine.h"

#	include <vector>
#	include <algorithm>

#	include "LogEngine.h"
#	include "FileSystem.h"

#	include "FactoryDefault.h"
#	include "FileSystemDirectory.h"
#	include "FileSystemZip.h"
#	include "FileInput.h"
#	include "FileOutput.h"

#	include "Core/String.h"
#	include "Core/File.h"

#	include "Consts.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	FileEngine::FileEngine()
		: m_interface(NULL)
		, m_baseDir("")
	{
	}
	//////////////////////////////////////////////////////////////////////////
	FileEngine::~FileEngine()
	{
		for( TFileSystemMap::iterator 
			it = m_fileSystemMap.begin(), 
			it_end = m_fileSystemMap.end();
		it != it_end;
		++it )
		{
			it->second->destroy();
		}

		//if( m_fileSystemMemoryMapped )
		//{
		//	m_fileSystemMemoryMapped->destroy();
		//}
		
		if( m_interface != NULL )
		{
			releaseInterfaceSystem( m_interface );
			m_interface = NULL;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool FileEngine::initialize()
	{
		FactoryManager::registerFactory( Consts::get()->c_dir, new FactoryDefault<FileSystemDirectory>() );
		FactoryManager::registerFactory( Consts::get()->c_pak, new FactoryDefault<FileSystemZip>() );
		FactoryManager::registerFactory( Consts::get()->c_zip, new FactoryDefault<FileSystemZip>() );
		//FactoryManager::registerFactory( Consts::get()->c_memory, new FactoryDefault<FileSystemMemoryMapped>() );

		//m_fileSystemMemoryMapped = FactoryManager::createObjectT<FileSystemMemoryMapped>( Consts::get()->c_memory );

		//if( m_fileSystemMemoryMapped->initialize( Utils::emptyString(), this, false ) == false )
		//{
		//	return false;
		//}

		bool result = initInterfaceSystem( &m_interface );
		if( ( result == false )|| ( m_interface == NULL ) )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool FileEngine::mountFileSystem( const ConstString& _fileSystemName, const String& _path, const ConstString & _type, bool _create )
	{
        MENGE_LOG_INFO( "FileEngine:mountFileSystem _fileSystemName '%s' _path '%s' _type '%s'"
                       , _fileSystemName.c_str() 
                       , _path.c_str()
                       , _type.c_str()
                       );
        
		TFileSystemMap::iterator it_find = m_fileSystemMap.find( _fileSystemName );
		if( it_find != m_fileSystemMap.end() )
		{
			MENGE_LOG_ERROR( "FileEngine::mountFileSystem FileSystem with name '%s' is already mount\n"
				"Remount would be performed"
				, _fileSystemName.c_str() 
				);

			return false;
		}

		FileSystem * fs = FactoryManager::createObjectT<FileSystem>( _type );

		if( fs == NULL )
		{
			MENGE_LOG_ERROR( "Error: (FileEngine::mountFileSystem) can't find FileSystem for object '%s'"
				, _path.c_str() 
				);

			return false;

			// try mount as Directory
			//fs = FactoryManager::createObjectT<FileSystem>( Consts::get()->c_builtin_empty );
		}

		if( fs->initialize( _path, this, _create ) == false )
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
	void FileEngine::unmountFileSystem( const ConstString& _fileSystemName )
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
	bool FileEngine::getFullPath( const ConstString& _fileSystemName, const ConstString & _path, String & _fullPath ) const
	{
		if( this->getFileSystemPath( _fileSystemName, _fullPath ) == false )
		{
			return false;
		}

		_fullPath += "/";
		_fullPath += Helper::to_str(_path);

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool FileEngine::getFileSystemPath( const ConstString& _fileSystemName, String & _fileSystemPath ) const
	{
		if( _fileSystemName.empty() )
		{
			return false;
		}


		TFileSystemMap::const_iterator it_find = m_fileSystemMap.find( _fileSystemName );
		if( it_find == m_fileSystemMap.end() )
		{
			return false;
		}

		_fileSystemPath = it_find->second->getPath();

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool FileEngine::existFile( const ConstString& _fileSystemName, const String& _filename )
	{
		TFileSystemMap::iterator it_find = m_fileSystemMap.find( _fileSystemName );
		if( it_find == m_fileSystemMap.end() )
		{
			return false;
		}
		
		FileSystem * fileSystem = it_find->second;
        
        MENGE_LOG_INFO( "FileEngine::existFile '%s' '%s'"
                       , _fileSystemName.c_str()
                       , _filename.c_str()
                       );
		
		return fileSystem->existFile( _filename );		
	}
	//////////////////////////////////////////////////////////////////////////
	FileInputStreamInterface* FileEngine::createInputFile( const ConstString& _fileSystemName )
	{
		TFileSystemMap::iterator it_find = m_fileSystemMap.find( _fileSystemName );
		if( it_find == m_fileSystemMap.end() )
		{
			MENGE_LOG_ERROR( "Error: (FileEngine::createInputFile) FileSystem '%s' not mount"
				, _fileSystemName.c_str()
				);

			return NULL;
		}

		FileSystem * fileSystem = it_find->second;
		FileInputStreamInterface* file = fileSystem->createInputFile();

		return file;
	}
	//////////////////////////////////////////////////////////////////////////
	FileInputStreamInterface* FileEngine::openInputFile( const ConstString& _fileSystemName, const String& _filename )
	{
        MENGE_LOG_INFO( "FileEngine::openInputFile '%s' '%s'"
                       , _fileSystemName.c_str()
                       , _filename.c_str()
                       );
        
		FileInputStreamInterface * file = this->createInputFile( _fileSystemName );

		if( file == 0 )
		{
			return 0;
		}

		if( file->open( _filename ) == false )
		{
			file->close();
			return 0;
		}		

		return file;
	}
	//////////////////////////////////////////////////////////////////////////
	FileOutputStreamInterface * FileEngine::createOutputFile( const ConstString& _fileSystemName )
	{
		TFileSystemMap::iterator it_find = m_fileSystemMap.find( _fileSystemName );
		if( it_find == m_fileSystemMap.end() )
		{
			MENGE_LOG_ERROR( "Error: (FileEngine::createInputFile) FileSystem '%s' not mount"
				, _fileSystemName.c_str()
				);

			return NULL;
		}

		FileOutputStreamInterface * file = it_find->second->createOutputFile();

		return file;
	}
	//////////////////////////////////////////////////////////////////////////
	FileOutputStreamInterface* FileEngine::openOutputFile( const ConstString& _fileSystemName, const String& _filename )
	{
		FileOutputStreamInterface * file = this->createOutputFile( _fileSystemName );

		if( file == 0 )
		{
			return 0;
		}

		if( file->open( _filename ) == false )
		{
			file->close();
			file = NULL;
		}

		return file;
	}
	//////////////////////////////////////////////////////////////////////////
	void FileEngine::setBaseDir( const String& _baseDir )
	{
		m_baseDir = _baseDir;
	}
	//////////////////////////////////////////////////////////////////////////
	const String& FileEngine::getBaseDir() const
	{
		return m_baseDir;
	}
	bool FileEngine::createDirectoryPathFileSystem_( FileSystem * _fs, const String& _path )
	{
		String dir_path = _path;

		String::size_type idx = 0;
		idx = dir_path.find( '/', idx );
		while( idx != String::npos )
		{
			String subDir = dir_path.substr( 0, idx );

			if( _fs->existFile( subDir ) == false &&
				_fs->createDirectory( subDir ) == false )
			{
				return false;
			}

			idx = dir_path.find( '/', idx+1 );
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool FileEngine::createDirectory( const ConstString& _fileSystemName, const String& _path )
	{
		TFileSystemMap::iterator it_find = m_fileSystemMap.find( _fileSystemName );
		if( it_find == m_fileSystemMap.end() )
		{
			MENGE_LOG_ERROR( "Error: (FileEngine::createDirectory) FileSystem '%s' not mount"
				, _fileSystemName.c_str() 
				);

			return false;
		}

		FileSystem * fs = it_find->second;

		if( this->createDirectoryPathFileSystem_( fs, _path ) == false )
		{
			return false;
		}

		if( fs->createDirectory( _path ) == false )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void FileEngine::removeDirectory( const ConstString& _fileSystemName, const String& _path )
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
	void FileEngine::removeFile( const ConstString& _fileSystemName, const String& _filename )
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
