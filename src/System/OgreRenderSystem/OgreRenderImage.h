#	pragma once

#	include "Interface/RenderSystemInterface.h"

#	include "Ogre.h"

class OgreRenderImage
	: public RenderImageInterface
{
public:
	OgreRenderImage( const TextureDesc&	_desc );
	~OgreRenderImage();

public:
	size_t getWidth() const override;
	size_t getHeight() const override;

public:
	Ogre::Texture * getTexture() const;

private:
	Ogre::TexturePtr m_texture;
	Ogre::String m_description;
};