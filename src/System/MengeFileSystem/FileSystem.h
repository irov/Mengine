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

	void loadPath( const Menge::String& _path ) override;
	void loadPak( const Menge::String& _pak ) override;
	void unloadPak( const Menge::String& _pak ) override;

	DataStreamInterface* openFile( const Menge::String& _filename ) override;
	DataStreamInterface* createMemoryFile( void* _data, std::size_t _size, bool _freeOnClose ) override;
	void closeStream( DataStreamInterface * _fd ) override;
	bool existFile( const Menge::String& _filename  ) override;

	const char * platformBundlePath() override;

	bool createFolder( const Menge::String& _path ) override;
	bool deleteFolder( const Menge::String& _path ) override;
	bool changeDir( const Menge::String& _path ) override;

	bool initAppDataPath( const Menge::String& _game ) override;
	const Menge::String& getAppDataPath() override;

private:
	FileManager* m_fileManager;
	Menge::String m_appDataPath;
};