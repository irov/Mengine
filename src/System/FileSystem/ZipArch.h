#pragma once 

#include <string>

class FileDataInterface;

class ZipArchive  
{ 
public:
	ZipArchive(const std::string& _filename, int _prior = -1);
	~ZipArchive();

public:
	FileDataInterface* fileRead(const std::string& _filename) const;
	bool haveFile(const char* _filename) const;
	bool loadZip(const std::string& _filename, int _prior);
	bool unloadZip(); 
	int getPriority() const;
	std::string getZipName() const;
private:
	void* m_uzFile;
	int m_priority;	
	std::string m_zipName;
};