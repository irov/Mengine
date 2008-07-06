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
	void FileEngine::loadPath( const std::string& _path )
	{
		m_interface->loadPath( _path.c_str() );
	}
	//////////////////////////////////////////////////////////////////////////
	void FileEngine::loadPak( const std::string& _filename )
	{
		m_interface->loadPak( _filename.c_str() );
	}
	//////////////////////////////////////////////////////////////////////////
	void FileEngine::unloadPak( const std::string& _filename )
	{
		m_interface->unloadPak( _filename.c_str() );
	}
	//////////////////////////////////////////////////////////////////////////
	bool FileEngine::existFile( const std::string& _filename )
	{
		return m_interface->existFile( _filename.c_str() );
	}
	//////////////////////////////////////////////////////////////////////////
	DataStreamInterface* FileEngine::openFile( const std::string& _filename )
	{
		return m_interface->openFile( _filename.c_str() );
	}
	//////////////////////////////////////////////////////////////////////////
	void FileEngine::closeStream( DataStreamInterface* _stream )
	{
		m_interface->closeStream( _stream );
	}
	//////////////////////////////////////////////////////////////////////////
	FileBuffer FileEngine::getFileBuffer( const std::string& _filename )
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
	bool FileEngine::createFolder( const std::string& _path )
	{
		return m_interface->createFolder( _path.c_str() );
	}
	//////////////////////////////////////////////////////////////////////////
	bool FileEngine::deleteFolder( const std::string& _path )
	{
		return m_interface->deleteFolder( _path.c_str() );
	}
	//////////////////////////////////////////////////////////////////////////
	bool FileEngine::changeDir( const std::string& _dir )
	{
		return m_interface->changeDir( _dir.c_str() );
	}
	//////////////////////////////////////////////////////////////////////////
	DataStreamInterface * FileEngine::createMemoryFile( void * _data, std::size_t _size, bool _freeOnClose )
	{
		return m_interface->createMemoryFile( _data, _size, _freeOnClose );
	}
	//////////////////////////////////////////////////////////////////////////
	bool FileEngine::initAppDataPath( const Menge::String& _game )
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