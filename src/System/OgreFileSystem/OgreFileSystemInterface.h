#	pragma once

#	include "Interface/FileSystemInterface.h"

#	include "Ogre.h"

class OgreFileSystemInterface
	: public FileSystemInterface
{
public:
	virtual void init( Ogre::ResourceGroupManager * _resourceMgr ) = 0;
};