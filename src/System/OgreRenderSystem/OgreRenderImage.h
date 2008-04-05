#	pragma once

#	include "Interface/RenderSystemInterface.h"

#	include "Ogre.h"

class OgreRenderImage
	: public RenderImageInterface
{
public:
	OgreRenderImage();
	OgreRenderImage( const std::string & _description);
	OgreRenderImage( const char* _description, unsigned int _width, unsigned int _height, Ogre::TextureUsage _usage, Ogre::ManualResourceLoader* _loader );
	OgreRenderImage( const TextureDesc&	_desc );
	~OgreRenderImage();

public:
	float getWidth() const override;
	float getHeight() const override;
	void writeToFile( const char* _filename ) override;
	const char * getDescription() const override;

public:
	Ogre::Texture * getTexture() const;

	Ogre::TexturePtr m_texture;

private:
	
	Ogre::String m_description;
};