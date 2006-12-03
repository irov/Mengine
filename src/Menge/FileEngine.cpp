#	include "FileEngine.h"

#	include "FileSystemInterface.h"

#	include "XmlParser.h"

namespace Menge
{
	FileEngine::FileEngine(const std::string &_dllModule)
		: DllModuleInterface<FileSystemInterface>(_dllModule)
	{
		Keeper<FileEngine>::keep(this);
	}

	bool FileEngine::loadPak(const std::string&	_filename, int _prior)
	{
		return m_interface->loadPak(_filename,_prior);
	}

	bool FileEngine::unloadPak(const std::string& _filename)
	{
		return m_interface->unloadPak(_filename);
	}

	bool FileEngine::existFile(const std::string& _filename)
	{
		return m_interface->existFile(_filename);
	}

	bool FileEngine::createFolder(const std::string& _foldername)
	{
		return m_interface->createFolder(_foldername);
	}

	FileDataInterface* FileEngine::openFile(const std::string& _filename)
	{
		return m_interface->openFile(_filename);
	}

	void FileEngine::closeFile(FileDataInterface* _fd)
	{
		m_interface->closeFile(_fd);
	}

	bool FileEngine::loadXml(TiXmlDocument &_xmlDocument, const std::string &_fileName)
	{
		// Delete the existing data:
		_xmlDocument.Clear();
		// There was a really terrifying little bug here. The code:
		//		value = filename
		// in the STL case, cause the assignment method of the std::string to
		// be called. What is strange, is that the std::string had the same
		// address as it's c_str() method, and so bad things happen. Looks
		// like a bug in the Microsoft STL implementation.
		// See STL_STRING_BUG above.
		// Fixed with the StringToBuffer class.

		FileDataInterface* file = openFile(_fileName);

		if ( file == 0 )
		{
			_xmlDocument.SetError( TiXmlBase::TIXML_ERROR_OPENING_FILE, 0, 0, TIXML_ENCODING_UNKNOWN );
			return false;
		}

		// Get the file size, so we can pre-allocate the string. HUGE speed impact.
		size_t length = file->size();

		// Strange case, but good to handle up front.
		if ( length == 0 )
		{
			closeFile(file);
			return false;
		}

		// If we have a file, assume it is all one big XML file, and read it in.
		// The document parser may decide the document ends sooner than the entire file, however.
		std::string data;
		data.reserve(length);

		const int BUF_SIZE = 2048;
		char buf[BUF_SIZE];

		while( file->read(buf,BUF_SIZE,1))
		{
			data += buf;
		}

		closeFile(file);

		_xmlDocument.Parse( data.c_str(), 0, TIXML_DEFAULT_ENCODING );

		if( _xmlDocument.Error() == true )
		{
			return false;
		}

		return true;
	}
}

void intrusive_ptr_add_ref(FileDataInterface * p)
{
	
}

void intrusive_ptr_release(FileDataInterface * p)
{

}


