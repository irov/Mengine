#pragma once
#	include "interfaces.h"
// STL
#include <vector>
#include <algorithm>
// C
#include <direct.h>
// 
#include "FileData.h"
#include "ZipArch.h"

class	FileSystem : public FileSystemInterface
{
public:
 	FileSystem();
 	~FileSystem();
	bool		createFolder(const std::string& _foldername);
 	bool		loadPak(const std::string& _filename, int _prior);
 	bool		unloadPak(const std::string& _filename);
  	bool		existFile(const std::string& _filename);
 	FileDataInterface*	openFile(const std::string& _filename);
	void		closeFile(FileDataInterface* _fd);
private:
	std::vector<ZipArchive*>		mZipArchives;
};