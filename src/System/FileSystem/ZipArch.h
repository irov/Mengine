#pragma once 

#include <string>

class FileDataInterface;

class ZipArchive  
{ 
public:
	ZipArchive(const std::string& _filename, int _prior = -1);
	~ZipArchive();

public:
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