#	pragma once

#	include "Ogre.h"

#	include "Interface\FileSystemInterface.h"

class OgreFileSystem
	: public FileSystemInterface
{
public:
	OgreFileSystem();

public:
	void loadPath( const char * _path ) override;
	void loadPak(const char * _pak ) override;
	void unloadPak(const char * _pak ) override;

	FileDataInterface *	openFile(const char *	_filename) override;
	void closeFile(FileDataInterface* _fd) override;
	bool existFile(const char * _filename) override;

	const char * platformBundlePath() override;

	bool createFolder( const char* _path ) override;

private:
	Ogre::Archive * m_arch;
};