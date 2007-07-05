#	pragma once

#	include "SystemDLL.h"
#	include "Holder.h"

#	include "FileSystemInterface.h"

class TiXmlDocument;

namespace Menge
{
	class FileEngine
		: public SystemDLL<FileSystemInterface>
	{
	public:
		FileEngine(const std::string &_dllModule);

	public:
		bool	loadPak(const std::string&	_filename, int _prior);
		bool	unloadPak(const std::string& _filename);
		bool	existFile(const std::string& _filename);
		bool	createFolder(const std::string&	_foldername);
		FileDataInterface*	openFile(const std::string&	_filename);
		void	closeFile(FileDataInterface* _fd);

	public:
		bool loadXml(TiXmlDocument &_xmlDocument, const std::string &_fileName);
	};
}