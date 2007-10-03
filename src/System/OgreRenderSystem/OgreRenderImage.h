#	pragma once

#	include "Interface/RenderSystemInterface.h"

#	include "Ogre.h"

class OgreRenderImage
	: public RenderImageInterface
{
public:
	OgreRenderImage( const TextureDesc&	_desc );

public:
	size_t getWidth() const override;
	size_t getHeight() const override;

public:
	Ogre::Texture * getTexture();

private:
	Ogre::TexturePtr m_texture;
};