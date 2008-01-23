#	include "OgreRenderVideoStream.h"

OgreRenderVideoStream::OgreRenderVideoStream( Ogre::TheoraMovieClip* _clip, const std::string& _name )
: m_clip( _clip )
, m_name( _name )
{

}

OgreRenderVideoStream::~OgreRenderVideoStream()
{

}

const std::string& OgreRenderVideoStream::getName()
{
	return m_name;
}

void OgreRenderVideoStream::play()
{
	m_clip->changePlayMode( Ogre::TextureEffectPlay_ASAP );
}

void OgreRenderVideoStream::pause()
{
	m_clip->changePlayMode( Ogre::TextureEffectPause );
}

size_t OgreRenderVideoStream::getWidth() const
{
	return m_texture->getWidth();
}

size_t OgreRenderVideoStream::getHeight() const
{
	return m_texture->getHeight();
}

void OgreRenderVideoStream::writeToFile( const char* _filename )
{

}