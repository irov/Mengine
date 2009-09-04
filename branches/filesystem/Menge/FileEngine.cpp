#	include "FileEngine.h"

#	include <vector>
#	include <algorithm>
#	include <cassert>

#	include "LogEngine.h"
#	include "FileSystem.h"
#	include "Utils.h"

#	include "FactoryDefault.h"
#	include "FileSystemDirectory.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	FileEngine::FileEngine( FileSystemInterface * _interface )
		: m_interface( _interface )
		, m_baseDir( "." )
	{
		m_fileSystemFactoryMgr.registerFactory( "", new FactoryDefault<FileSystemDirectory>() );
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
		m_fileSystemMap.clear();
	}
	//////////////////////////////////////////////////////////////////////////
	bool FileEngine::mountFileSystem( const String& _fileSystemName, const String& _path, bool _create )
	{
		TFileSystemMap::iterator it_find = m_fileSystemMap.find( _fileSystemName );
		if( it_find != m_fileSystemMap.end() )
		{
			MENGE_LOG( "Warning: (FileEngine::mountFileSystem) FileSystem with name \"%s\" is already mount\n"
				"Remount would be performed",
				_fileSystemName.c_str() );
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
		FileSystem* fs = m_fileSystemFactoryMgr.createObjectT<FileSystem>( typeExt );
		if( fs == NULL )
		{
			MENGE_LOG_ERROR( "Error: (FileEngine::mountFileSystem) can't find FileSystem for object \"%s\"",
				_path.c_str() );
			return false;
		}

		String fullpath = _path;

		if( s_isAbsolutePath( _path ) == false )
		{
			fullpath = m_baseDir + "/" + fullpath;
		}

		if( fs->initialize( fullpath, _create ) == false )
		{
			MENGE_LOG_ERROR( "Error: (FileEngine::mountFileSystem) can't initialize FileSystem for object \"%s\"",
				_path.c_str() );
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
			MENGE_LOG_ERROR( "Error: (FileEngine::unmountFileSystem) FileSystem with name \"%s\" not mount",
				_fileSystemName.c_str() );
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
	FileInputInterface* FileEngine::openFileInput( const String& _fileSystemName, const String& _filename )
	{
		TFileSystemMap::iterator it_find = m_fileSystemMap.find( _fileSystemName );
		if( it_find == m_fileSystemMap.end() )
		{
			MENGE_LOG_ERROR( "Error: (FileEngine::openFileInput) FileSystem \"%s\" not mount",
				_fileSystemName.c_str() );
			return NULL;
		}

		if( it_find->second->existFile( _filename ) == false )
		{
			MENGE_LOG_ERROR( "Error: (FileEngine::openFileInput) file not found \"%s\": \"%s\"",
				_fileSystemName.c_str(), _filename.c_str() );
			return NULL;
		}

		MENGE_LOG( "-- Open File %s", _filename.c_str() );
		FileInputInterface* file = it_find->second->openInputFile( _filename );

		return file;
	}
	//////////////////////////////////////////////////////////////////////////
	void FileEngine::closeFileInput( FileInputInterface* _file )
	{
		_file->close();
	}
	//////////////////////////////////////////////////////////////////////////
	FileOutputInterface* FileEngine::openFileOutput( const String& _fileSystemName, const String& _filename )
	{
		TFileSystemMap::iterator it_find = m_fileSystemMap.find( _fileSystemName );
		if( it_find == m_fileSystemMap.end() )
		{
			MENGE_LOG_ERROR( "Error: (FileEngine::openFileOutput) FileSystem \"%s\" not mount",
				_fileSystemName.c_str() );
			return NULL;
		}

		FileOutputInterface* file = it_find->second->openOutputFile( _filename );

		return file;
	}
	//////////////////////////////////////////////////////////////////////////
	void FileEngine::closeFileOutput( FileOutputInterface* _file )
	{
		_file->close();
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
	//////////////////////////////////////////////////////////////////////////
	bool FileEngine::createDirectory( const String& _fileSystemName, const String& _path )
	{
		TFileSystemMap::iterator it_find = m_fileSystemMap.find( _fileSystemName );
		if( it_find == m_fileSystemMap.end() )
		{
			MENGE_LOG_ERROR( "Error: (FileEngine::createDirectory) FileSystem \"%s\" not mount",
				_fileSystemName.c_str() );
			return false;
		}

		FileSystem* fs = it_find->second;

		String::size_type idx = 0;
		idx = _path.find( '/', idx );
		while( idx != String::npos )
		{
			String subDir = _path.substr( 0, idx );
			if( fs->createDirectory( subDir ) == false )
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
			MENGE_LOG_ERROR( "Error: (FileEngine::removeDirectory) FileSystem \"%s\" not mount",
				_fileSystemName.c_str() );
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
			MENGE_LOG_ERROR( "Error: (FileEngine::removeFile) FileSystem \"%s\" not mount",
				_fileSystemName.c_str() );
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
