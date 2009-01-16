#	include "FileEngine.h"

#	include <vector>

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
	}
	//////////////////////////////////////////////////////////////////////////
	void FileEngine::loadPath( const String& _path )
	{
		m_interface->loadPath( _path );
	}
	//////////////////////////////////////////////////////////////////////////
	void FileEngine::loadPak( const String& _filename )
	{
		m_interface->loadPak( _filename );
	}
	//////////////////////////////////////////////////////////////////////////
	void FileEngine::unloadPak( const String& _filename )
	{
		m_interface->unloadPak( _filename );
	}
	//////////////////////////////////////////////////////////////////////////
	bool FileEngine::existFile( const String& _filename )
	{
		return m_interface->existFile( _filename );
	}
	//////////////////////////////////////////////////////////////////////////
	bool FileEngine::deleteFile( const String& _filename )
	{
		return m_interface->deleteFile( _filename );
	}
	//////////////////////////////////////////////////////////////////////////
	DataStreamInterface * FileEngine::openFile( const String& _filename )
	{
		return m_interface->openFile( _filename );
	}
	//////////////////////////////////////////////////////////////////////////
	void FileEngine::closeStream( DataStreamInterface* _stream )
	{
		m_interface->closeStream( _stream );
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
		return m_interface->createFolder( _path );
	}
	//////////////////////////////////////////////////////////////////////////
	bool FileEngine::deleteFolder( const String& _path )
	{
		return m_interface->deleteFolder( _path );
	}
	//////////////////////////////////////////////////////////////////////////
	DataStreamInterface * FileEngine::createMemoryFile( void * _data, std::streamsize _size, bool _freeOnClose )
	{
		return m_interface->createMemoryFile( _data, _size, _freeOnClose );
	}
	//////////////////////////////////////////////////////////////////////////
	bool FileEngine::initAppDataPath( const String& _game )
	{
		return m_interface->initAppDataPath( _game );
	}
	//////////////////////////////////////////////////////////////////////////
	const String& FileEngine::getAppDataPath()
	{
		return m_interface->getAppDataPath();
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
}
