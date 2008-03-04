#	include "FileEngine.h"

#	include <vector>

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	FileEngine::FileEngine( FileSystemInterface * _interface )
		: m_interface( _interface )
	{
		Holder<FileEngine>::keep( this );
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
	FileDataInterface* FileEngine::openFile( const std::string& _filename )
	{
		return m_interface->openFile( _filename.c_str() );
	}
	//////////////////////////////////////////////////////////////////////////
	void FileEngine::closeFile( FileDataInterface* _fd )
	{
		m_interface->closeFile( _fd );
	}
	//////////////////////////////////////////////////////////////////////////
	FileBuffer FileEngine::getFileBuffer( const std::string& _filename )
	{
		FileDataInterface * file = openFile( _filename );

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

		closeFile( file );

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
	void FileEngine::addResourceLocation( const std::string& _path )
	{
		m_interface->addResourceLocation( _path.c_str() );
	}
	//////////////////////////////////////////////////////////////////////////
	void FileEngine::initResources()
	{
		m_interface->initResources();
	}
}