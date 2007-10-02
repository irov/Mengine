#	include "FileEngine.h"

#	include "XmlParser/XmlParser.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	FileEngine::FileEngine( FileSystemInterface * _interface )
		: m_interface( _interface )
	{
		Holder<FileEngine>::keep(this);
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
		m_interface->unloadPak(_filename.c_str());
	}
	//////////////////////////////////////////////////////////////////////////
	bool FileEngine::existFile( const std::string& _filename )
	{
		return m_interface->existFile(_filename.c_str());
	}
	//////////////////////////////////////////////////////////////////////////
	FileDataInterface* FileEngine::openFile( const std::string& _filename )
	{
		return m_interface->openFile(_filename.c_str());
	}
	//////////////////////////////////////////////////////////////////////////
	void FileEngine::closeFile( FileDataInterface* _fd )
	{
		m_interface->closeFile(_fd);
	}
	//////////////////////////////////////////////////////////////////////////
	TiXmlDocument * FileEngine::loadXml( const std::string & _filename )
	{
		// There was a really terrifying little bug here. The code:
		//		value = filename
		// in the STL case, cause the assignment method of the std::string to
		// be called. What is strange, is that the std::string had the same
		// address as it's c_str() method, and so bad things happen. Looks
		// like a bug in the Microsoft STL implementation.
		// See STL_STRING_BUG above.
		// Fixed with the StringToBuffer class.

		FileDataInterface * file = openFile(_filename);

		if ( file == 0 )
		{
			return 0;
		}

		// Get the file size, so we can pre-allocate the string. HUGE speed impact.
		size_t length = file->size();

		// Strange case, but good to handle up front.
		if ( length == 0 )
		{
			closeFile(file);
			return 0;
		}

		// If we have a file, assume it is all one big XML file, and read it in.
		// The document parser may decide the document ends sooner than the entire file, however.

		char * buff = new char[ length ];
		file->read( buff, length );
		
		std::string data( buff, length );

		closeFile(file);

		TiXmlDocument * document = TiXmlDocumentLoadData( data );

		return document;
	}
}

