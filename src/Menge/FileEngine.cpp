#	include "FileEngine.h"

#	include <vector>

#	include "FilePackZip.h"
#	include "LogEngine.h"

#	include "MemoryStream.h"

#	include <algorithm>


#	include <assert.h>

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
		for( TFilePackMap::iterator it = m_packs.begin(), it_end = m_packs.end();
			it != it_end;
			++it )
		{
			delete it->second.pack;
			closeStream( it->second.stream );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void FileEngine::loadPath( const String& _path )
	{
		m_basePath = _path;
	}
	//////////////////////////////////////////////////////////////////////////
	void FileEngine::loadPak( const String& _filename )
	{
		// check for .pak
		if( _filename.find( ".pak" ) == String::npos )
		{
			return;
		}

		// check if already loaded
		for( TFilePackMap::iterator it = m_packs.begin(), it_end = m_packs.end();
			it != it_end;
			++it )
		{
			if( _filename == it->first )
			{
				return;
			}
		}

		DataStreamInterface* packStream = openFile( _filename, true );
		if( packStream == NULL )
		{
			MENGE_LOG_ERROR( "Error: (FileEngine::loadPack) failed to open \"%s\""
				, _filename.c_str() );
			return;
		}

		FilePackZip* zip = new FilePackZip( _filename, packStream );
		if( zip->getFileCount() == 0 )
		{
			MENGE_LOG_ERROR( "Error: (FileEngine::loadPack) unsupported type or invalid pack \"%s\""
				, _filename.c_str() );
			delete zip;
		}
		else
		{
			PackInfo pkInfo = { zip, packStream };
			m_packs.insert( std::make_pair( _filename, pkInfo ) );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void FileEngine::unloadPak( const String& _filename )
	{
		//m_interface->unloadPak( _filename );
		TFilePackMap::iterator it_find = m_packs.find( _filename );
		if( it_find != m_packs.end() )
		{
			delete it_find->second.pack;
			closeStream( it_find->second.stream );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool FileEngine::existFile( const String& _filename )
	{
		// search for packs first
		for( TFilePackMap::iterator it = m_packs.begin(), it_end = m_packs.end();
			it != it_end;
			++it )
		{
			if( _filename.find( it->first ) == 0 )	// this is must be a pack
			{
				return it->second.pack->hasFile( _filename.substr( it->first.length() + 1 ) );
			}
		}

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
	DataStreamInterface * FileEngine::openFile( const String& _filename, bool _map/* = false*/ )
	{
		MENGE_LOG( "Opening file: \"%s\"", _filename.c_str() );

#ifndef MENGE_MASTER_RELEASE
		// check filename delims
		if( _filename.find( '\\' ) != String::npos )
		{
			MENGE_LOG_ERROR( "Warning: Delimiter mismatch in \"%s\" ('\\' instead of '/')"
				, _filename.c_str() );
		}
#endif
		// search for packs first
		for( TFilePackMap::iterator it = m_packs.begin(), it_end = m_packs.end();
			it != it_end;
			++it )
		{
			if( _filename.find( it->first ) == 0 )	// this is must be a pack
			{
				if( _filename == it->first )	// root
				{
					return it->second.stream;
				}
				return it->second.pack->openFile( _filename.substr( it->first.length() + 1 ) );
			}
		}

 		//if( m_interface->existFile( _filename ) == false )
 		//{
 		//	MENGE_LOG_ERROR( "file \"%s\" does not exist", _filename.c_str() );
 		//	return NULL;
 		//}
		String fullFilename = _filename;
		if( m_basePath.empty() == false
			&& ( _filename[0] != '/' && _filename[1] != ':' ) )
		{
			fullFilename = m_basePath + "/" + _filename;
		}

		return m_interface->openFile( fullFilename, _map );
	}
	//////////////////////////////////////////////////////////////////////////
	void FileEngine::closeStream( DataStreamInterface* _stream )
	{
		_stream->release();
		//m_interface->closeStream( _stream );
	}
	//////////////////////////////////////////////////////////////////////////
	FileBuffer FileEngine::getFileBuffer( const String& _filename )
	{
		DataStreamInterface * file = openFile( _filename );

		FileBuffer fileBuffer = {NULL, 0};

		if( file == 0 )
		{
			return fileBuffer;
		}
	
		fileBuffer.size = file->size();
		m_fileCache.resize( fileBuffer.size + 2 );
		fileBuffer.buffer = &m_fileCache[0];

		file->read( fileBuffer.buffer, fileBuffer.size );

		closeStream( file );

		return fileBuffer;
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
		if( isAbsolutePath( _path ) == true )
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
	DataStreamInterface * FileEngine::createMemoryFile( void * _data, std::streamsize _size )
	{
		//return m_interface->createMemoryFile( _data, _size );
		return new MemoryStream( _data, _size );
	}
	//////////////////////////////////////////////////////////////////////////
	bool FileEngine::initAppDataPath( const String& _game, bool _local )
	{
		bool result = m_interface->initAppDataPath( _game, _local );
		if( result == true )
		{
			m_userPath = m_interface->getAppDataPath();
			std::replace( m_userPath.begin(), m_userPath.end(), '\\', '/' );
		}
		return result;
	}
	//////////////////////////////////////////////////////////////////////////
	const String& FileEngine::getAppDataPath()
	{
		return m_userPath;
	}
	//////////////////////////////////////////////////////////////////////////
	OutStreamInterface * FileEngine::openOutStream( const String& _filename, bool _binary )
	{
		return m_interface->openOutStream( _filename, _binary );
	}
	//////////////////////////////////////////////////////////////////////////
	void FileEngine::closeOutStream( OutStreamInterface * _outStream )
	{
		m_interface->closeOutStream( _outStream );
	}
	//////////////////////////////////////////////////////////////////////////
	bool FileEngine::isAbsolutePath( const String& _path )
	{
		return m_interface->isAbsolutePath( _path );
	}
	//////////////////////////////////////////////////////////////////////////
	String FileEngine::joinPath( const String& _base, const String& _name )
	{
		return m_interface->joinPath( _base, _name );
	}
	//////////////////////////////////////////////////////////////////////////
	const String& FileEngine::getBasePath() const
	{
		return m_basePath;
	}
	//////////////////////////////////////////////////////////////////////////
}
