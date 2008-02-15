#	pragma once

#	include "Interface/RenderSystemInterface.h"

#	include "Ogre.h"

class	OgreRenderSystemInterface
	: public RenderSystemInterface
{
public:
	virtual bool init( Ogre::Root * _root, Ogre::RenderWindow * _renderWindow ) = 0;
};