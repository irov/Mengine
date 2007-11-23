#	include "FileEngine.h"

#	include "XmlParser/XmlParser.h"

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
	TiXmlDocument * FileEngine::loadXml( const std::string & _filename )
	{
		FileDataInterface * file = openFile( _filename );

		if ( file == 0 )
		{
			return 0;
		}

		size_t length = file->size();

		std::string data;

		data.resize( length );

		file->read( &data[0], length );

		closeFile( file );

		TiXmlDocument * document = TiXmlDocumentLoadData( data );

		if ( document == 0 )
		{
			return 0;
		}

		return document;
	}
	//////////////////////////////////////////////////////////////////////////
	bool FileEngine::createFolder( const std::string& _path )
	{
		return m_interface->createFolder( _path.c_str() );
	}
}