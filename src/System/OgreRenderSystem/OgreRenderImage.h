#	pragma once

#	include "Interface/RenderSystemInterface.h"

#	include "Ogre.h"

class OgreRenderImage
	: public RenderImageInterface
{
public:
	OgreRenderImage( const char* _name, unsigned int _width, unsigned int _height );
	OgreRenderImage( const TextureDesc&	_desc );
	~OgreRenderImage();

public:
	size_t getWidth() const override;
	size_t getHeight() const override;

public:
	Ogre::Texture * getTexture() const;

	Ogre::TexturePtr m_texture;

private:
	
	Ogre::String m_description;
};