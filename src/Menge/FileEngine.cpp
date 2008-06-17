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
		DataStreamInterface* file = openFile( _filename );

		FileBuffer fb;

		fb.buffer = 0;
		fb.size = 0;

		if( file == 0 )
		{
			return fb;
		}
	
		fb.size = file->size();
		m_fileCache.resize( fb.size + 2 );
		fb.buffer = &m_fileCache[0];

		file->read( fb.buffer, fb.size );

		closeStream( file );

		return fb;
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
	DataStreamInterface* FileEngine::createMemoryFile( void* _data, std::size_t _size, bool _freeOnClose )
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
}