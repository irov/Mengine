#	pragma once

#	include "Interface/RenderSystemInterface.h"

#	include "Ogre.h"

class RenderImageInterface
{
public:
	virtual int getWidth() const = 0;
	virtual int getHeight() const = 0;
};

class OgreRenderImage
	: public RenderImageInterface
{
	OgreRenderImage( const TextureDesc&	_desc );

public:
	int getWidth() const override;
	int getHeight() const override;

private:
	Ogre::Image * m_image;
};