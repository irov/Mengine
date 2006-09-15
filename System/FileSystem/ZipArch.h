#pragma once 

#include <string>
#include "zlib\unzip.h"
#include "zlib\iowin32.h" 

#include "FileData.h"

class ZipArchive  
{ 
public:
	ZipArchive(const std::string& _filename, int _prior = -1);
	~ZipArchive();
	FileDataInterface*	FileRead(const std::string& _filename) const;
	bool			HaveFile(const std::string& _filename) const;
	bool			LoadZip(const std::string& _filename, int _prior);
	bool			UnloadZip(); 
	int				GetPriority() const;
	std::string		GetZipName() const;
private:
	void*			mUzFile;
	int				mPriority;	
	std::string		mZipName;
};