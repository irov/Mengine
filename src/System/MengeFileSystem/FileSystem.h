#	pragma once

#	include "Interface\FileSystemInterface.h"

class FileManager;

class MengeFileSystem
	: public FileSystemInterface
{
public:
	MengeFileSystem();
	~MengeFileSystem();

public:
	void loadPath( const char * _path ) override;
	void loadPak( const char * _pak ) override;
	void unloadPak( const char * _pak ) override;

	FileDataInterface *	openFile(const char * _filename) override;
	void closeFile( FileDataInterface * _fd ) override;
	bool existFile( const char * _filename  ) override;

	const char * platformBundlePath() override;

	bool createFolder( const char* _path ) override;
	bool deleteFolder( const char * _path ) override;
	bool changeDir( const char* _path ) override;

	const TCHAR* getApplicationDataPath( const TCHAR* _game ) override;

private:
	FileManager* m_fileManager;
};