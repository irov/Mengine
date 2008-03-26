#	pragma once

#	include "Interface/RenderSystemInterface.h"

#	include "Ogre.h"
#	include "TheoraMovieClip.h"

class OgreRenderVideoStream
	: public RenderVideoStreamInterface
{
public:
	OgreRenderVideoStream( Ogre::TheoraMovieClip* _clip, const std::string& _name );
	~OgreRenderVideoStream();

public:
	float getWidth() const override;
	float getHeight() const override;
	void writeToFile( const char* _filename ) override;
	void play();
	void pause();

public:
	Ogre::Texture * getTexture() const;

	Ogre::TexturePtr m_texture;

	const std::string& getName();

protected:
	Ogre::TheoraMovieClip* m_clip;
	std::string m_name;
};