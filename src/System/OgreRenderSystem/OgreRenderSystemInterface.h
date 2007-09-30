#	pragma once

#	include "Interface/RenderSystemInterface.h"

namespace Ogre
{
	class Root;
	class RenderWindow;
}

class	OgreRenderSystemInterface
	: public RenderSystemInterface
{
public:
	virtual bool init( Ogre::Root * _root, Ogre::RenderWindow * _renderWindow ) = 0;
};