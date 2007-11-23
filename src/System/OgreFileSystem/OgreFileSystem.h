#	pragma once

#	include "OgreFileSystemInterface.h"

class OgreFileSystem
	: public OgreFileSystemInterface
{
public:
	OgreFileSystem();

public:
	void init( Ogre::ResourceGroupManager * _resourceMgr ) override;

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
	Ogre::ResourceGroupManager * m_resourceMgr;
};