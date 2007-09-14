#	pragma once

#	include <string>

#	include "Interface/FileSystemInterface.h"

#	include "XmlParser/XmlParser.h"

#	include "Holder.h"

namespace Menge
{
	class FileEngine
	{
	public:
		FileEngine( FileSystemInterface * _interface );

	public:
		bool loadPak( const std::string& _filename, int _prior );
		bool unloadPak( const std::string& _filename );
		bool existFile( const std::string& _filename );
		bool createFolder( const std::string& _filename );

		FileDataInterface * openFile( const std::string& _filename );
		void closeFile( FileDataInterface* _fd );

	public:
		TiXmlDocument * loadXml( const std::string & _filename );

	protected:
		FileSystemInterface * m_interface;
	};
}