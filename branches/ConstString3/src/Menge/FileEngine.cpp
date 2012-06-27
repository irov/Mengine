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
		//FactoryManager::registerFactory( Consts::get()->c_pak, new FactoryDefault<FileSystemZip>() );
		//FactoryManager::registerFactory( Consts::get()->c_zip, new FactoryDefault<FileSystemZip>() );
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
	bool FileEngine::mountFileSystem( const ConstString& _fileSystemName, const WString& _path, const ConstString & _type, bool _create )
	{
        MENGE_LOG_INFO( "FileEngine:mountFileSystem _fileSystemName '%s' _path '%S' _type '%s'"
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
			MENGE_LOG_ERROR( "Error: (FileEngine::mountFileSystem) can't find FileSystem for object '%S'"
				, _path.c_str() 
				);

			return false;

			// try mount as Directory
			//fs = FactoryManager::createObjectT<FileSystem>( Consts::get()->c_builtin_empty );
		}

		if( fs->initialize( _path, this, _create ) == false )
		{
			MENGE_LOG_ERROR( "Error: (FileEngine::mountFileSystem) can't initialize FileSystem for object '%S'"
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
	bool FileEngine::getFullPath( const ConstString& _fileSystemName, const WString & _path, WString & _fullPath ) const
	{
		if( this->getFileSystemPath( _fileSystemName, _fullPath ) == false )
		{
			return false;
		}

		_fullPath += _path;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool FileEngine::getFileSystemPath( const ConstString& _fileSystemName, WString & _fileSystemPath ) const
	{
		if( _fileSystemName.empty() )
		{
			return false;
		}
		
		TFileSystemMap::const_iterator it_find = m_fileSystemMap.find( _fileSystemName );
		if( it_find == m_fileSystemMap.end() )
		{
			MENGE_LOG_WARNING("FileEngine::getFileSystemPath not found '%s'"
				, _fileSystemName.c_str()
				);

			return false;
		}

		_fileSystemPath = it_find->second->getPath();

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool FileEngine::existFile( const ConstString& _fileSystemName, const WString& _filename ) const
	{
		TFileSystemMap::const_iterator it_find = m_fileSystemMap.find( _fileSystemName );
		if( it_find == m_fileSystemMap.end() )
		{
			return false;
		}
		
		FileSystem * fileSystem = it_find->second;

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
	FileInputStreamInterface* FileEngine::openInputFile( const ConstString& _fileSystemName, const WString& _filename )
	{
		FileInputStreamInterface * file = this->createInputFile( _fileSystemName );

		if( file == 0 )
		{
			MENGE_LOG_ERROR("FileEngine::openInputFile can't create input file '%s'"
				, _fileSystemName.c_str()
				);

			return 0;
		}

		if( file->open( _filename ) == false )
		{
			file->close();

			MENGE_LOG_ERROR("FileEngine::openInputFile can't open input file '%s' '%S'"
				, _fileSystemName.c_str()
				, _filename.c_str()
				);

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
	FileOutputStreamInterface* FileEngine::openOutputFile( const ConstString & _fileSystemName, const WString& _filename )
	{
		FileOutputStreamInterface * file = this->createOutputFile( _fileSystemName );

		if( file == 0 )
		{
			MENGE_LOG_ERROR("FileEngine::openOutputFile can't create output file '%s'"
				, _fileSystemName.c_str()
				);

			return 0;
		}

		if( file->open( _filename ) == false )
		{
			file->close();

			MENGE_LOG_ERROR("FileEngine::openOutputFile can't open output file '%s' '%S'"
				, _fileSystemName.c_str()
				, _filename.c_str()
				);
			
			file = NULL;
		}

		return file;
	}
	//////////////////////////////////////////////////////////////////////////
	bool FileEngine::createDirectoryPathFileSystem_( FileSystem * _fs, const WString& _path ) const
	{
		WString dir_path = _path;

		WString::size_type idx = 0;
		idx = dir_path.find( L'\\', idx );
		if( idx == String::npos )
		{
			idx = dir_path.find( L'/', idx );
		}
		while( idx != String::npos )
		{
			WString subDir = dir_path.substr( 0, idx );

			if( _fs->existFile( subDir ) == false &&
				_fs->createDirectory( subDir ) == false )
			{
				return false;
			}

			//idx = dir_path.find( MENGE_FOLDER_DELIM, idx+1 );
			idx = dir_path.find( L'\\', idx + 1 );
			if( idx == String::npos )
			{
				idx = dir_path.find( L'/', idx + 1 );
			}
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool FileEngine::createDirectory( const ConstString& _fileSystemName, const WString& _path )
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
	void FileEngine::removeDirectory( const ConstString& _fileSystemName, const WString& _path )
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
	void FileEngine::removeFile( const ConstString& _fileSystemName, const WString& _filename )
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
	FileSystemInterface* FileEngine::getFileSystemInterface() const
	{
		return m_interface;
	}
	//////////////////////////////////////////////////////////////////////////
}
