#	include "FileEngine.h"

#	include <vector>
#	include <algorithm>
#	include <cassert>

#	include "LogEngine.h"
#	include "FileSystem.h"
#	include "Utils.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	FileEngine::FileEngine( FileSystemInterface * _interface )
		: m_interface( _interface )
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

		String typeExt;
		Utils::getFileExt( typeExt, _path );
		FileSystem* fs = m_fileSystemFactoryMgr.createObjectT<FileSystem>( typeExt );
		if( fs == NULL )
		{
			MENGE_LOG_ERROR( "Error: (FileEngine::mountFileSystem) can't find FileSystem for object \"%s\"",
				_path.c_str() );
			return false;
		}

		String fullpath = m_baseDir + "/" + _path;
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
	FileInterface* FileEngine::openFile( const String& _fileSystemName, const String& _filename )
	{
		TFileSystemMap::iterator it_find = m_fileSystemMap.find( _fileSystemName );
		if( it_find == m_fileSystemMap.end() )
		{
			MENGE_LOG_ERROR( "Error: (FileEngine::openFile) FileSystem \"%s\" not mount",
				_fileSystemName.c_str() );
			return NULL;
		}

		if( it_find->second->existFile( _filename ) == false )
		{
			MENGE_LOG_ERROR( "Error: (FileEngine::openFile) file not found \"%s\": \"%s\"",
				_fileSystemName.c_str(), _filename.c_str() );
			return NULL;
		}

		FileInterface* file = it_find->second->openFile( _filename );

		return file;
	}
	//////////////////////////////////////////////////////////////////////////
	void FileEngine::closeFile( FileInterface* _file )
	{
		_file->close();
	}
	//////////////////////////////////////////////////////////////////////////
	OutStreamInterface * FileEngine::openOutStream( const String& _filename, bool _binary )
	{
		String fullname = _filename;
		if( m_userPath.empty() == false && s_isAbsolutePath_( _filename ) == false )
		{
			fullname = m_userPath + "/" + _filename;
		}
		return m_interface->openOutStream( fullname, _binary );
	}
	//////////////////////////////////////////////////////////////////////////
	void FileEngine::closeOutStream( OutStreamInterface * _outStream )
	{
		m_interface->closeOutStream( _outStream );
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
	bool FileEngine::existFile( const String& _filename )
	{
		String fullFilename = _filename;
		if( m_basePath.empty() == false
			&& ( _filename[0] != '/' && _filename[1] != ':' ) )
		{
			fullFilename = m_basePath + "/" + _filename;
		}

		return m_interface->existFile( fullFilename );
	}
	//////////////////////////////////////////////////////////////////////////
	bool FileEngine::deleteFile( const String& _filename )
	{
		return m_interface->deleteFile( _filename );
	}
	//////////////////////////////////////////////////////////////////////////
	DataStreamInterface* FileEngine::openStream( const String& _filename )
	{
		return m_interface->openStream( _filename );
	}
	//////////////////////////////////////////////////////////////////////////
	void FileEngine::closeStream( DataStreamInterface* _stream )
	{
		m_interface->closeStream( _stream );
	}
	//////////////////////////////////////////////////////////////////////////
	bool FileEngine::createFolder( const String& _path )
	{
		if( existFile( _path ) == true )
		{
			return true;
		}

		String::size_type idx = 0;
		idx = _path.find( '/', idx );
		if( s_isAbsolutePath_( _path ) == true )
		{
			idx = _path.find( '/', idx+1 );
		}
		while( idx != String::npos )
		{
			String subFolder = _path.substr( 0, idx );
			if( existFile( subFolder ) == false )
			{
				if( m_interface->createFolder( subFolder ) == false )
				{
					return false;
				}
			}
			idx = _path.find( '/', idx+1 );
		}
		if( m_interface->createFolder( _path ) == false )
		{
			return false;
		}
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool FileEngine::deleteFolder( const String& _path )
	{
		return m_interface->deleteFolder( _path );
	}
	//////////////////////////////////////////////////////////////////////////
}
