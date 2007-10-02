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
		void loadPath( const std::string& _path );
		void loadPak( const std::string& _filename );
		void unloadPak( const std::string& _filename );
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