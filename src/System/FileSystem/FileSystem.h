#	pragma once

#	include "Interface/FileSystemInterface.h"
#	include <vector>

class ZipArchive;
class FileDataInterface;

typedef std::vector<ZipArchive*> TVecZipArch;

class	FileSystem 
	: public FileSystemInterface
{
public:
 	FileSystem();
 	~FileSystem();

public:
	bool		createFolder(const char * _foldername);
 	bool		loadPak(const char * _filename, int _prior);
 	bool		unloadPak(const char * _filename);
  	bool		existFile(const char * _filename);
 	FileDataInterface*	openFile(const char * _filename);
	void		closeFile(FileDataInterface* _fd);

private:
	TVecZipArch m_zipArchives;
};