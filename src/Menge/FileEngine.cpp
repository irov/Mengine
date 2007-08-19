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
	bool FileEngine::loadPak(const std::string&	_filename, int _prior)
	{
		return m_interface->loadPak(_filename.c_str(),_prior);
	}
	//////////////////////////////////////////////////////////////////////////
	bool FileEngine::unloadPak(const std::string& _filename)
	{
		return m_interface->unloadPak(_filename.c_str());
	}
	//////////////////////////////////////////////////////////////////////////
	bool FileEngine::existFile(const std::string& _filename)
	{
		return m_interface->existFile(_filename.c_str());
	}
	//////////////////////////////////////////////////////////////////////////
	bool FileEngine::createFolder(const std::string& _foldername)
	{
		return m_interface->createFolder(_foldername.c_str());
	}
	//////////////////////////////////////////////////////////////////////////
	FileDataInterface* FileEngine::openFile(const std::string& _filename)
	{
		return m_interface->openFile(_filename.c_str());
	}
	//////////////////////////////////////////////////////////////////////////
	void FileEngine::closeFile(FileDataInterface* _fd)
	{
		m_interface->closeFile(_fd);
	}
	//////////////////////////////////////////////////////////////////////////
	TiXmlDocument * FileEngine::loadXml( const std::string &_fileName)
	{
		// There was a really terrifying little bug here. The code:
		//		value = filename
		// in the STL case, cause the assignment method of the std::string to
		// be called. What is strange, is that the std::string had the same
		// address as it's c_str() method, and so bad things happen. Looks
		// like a bug in the Microsoft STL implementation.
		// See STL_STRING_BUG above.
		// Fixed with the StringToBuffer class.

		FileDataInterface * file = openFile(_fileName);

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

		std::string data( file->getBuffer(), length );

		//printf("%s\n",data.c_str());
		//data.reserve(length);

		//const int BUF_SIZE = 2048;
		//char buf[BUF_SIZE + 1];
		//buf[BUF_SIZE] = '\0';

		//while( length > BUF_SIZE )
		//{
		//	file->read( buf, BUF_SIZE, 1 );
		//	data += buf;
		//	length -= BUF_SIZE;
		//}

		//if( length > 0 )
		//{
		//	file->read( buf, length, 1 );
		//	data += buf;
		//}

		////while( file->read(buf,BUF_SIZE,1))
		////{
		////	data += buf;
		////}

		//data += '\0';

		closeFile(file);

		TiXmlDocument * document = TiXmlDocumentLoadData( data );

		return document;
	}
}

