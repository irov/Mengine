#	include "OgreRenderImage.h"

//////////////////////////////////////////////////////////////////////////
OgreRenderImage::OgreRenderImage( const TextureDesc& _desc )
{
	Ogre::Image image;

	Ogre::DataStreamPtr data( new Ogre::MemoryDataStream( _desc.buffer, _desc.size ) );
	image.load( data, "png" );

	m_texture = Ogre::TextureManager::getSingletonPtr()
		->loadImage( _desc.name, "Default", image );
}
//////////////////////////////////////////////////////////////////////////
size_t OgreRenderImage::getWidth() const
{
	return m_texture->getWidth();
}
//////////////////////////////////////////////////////////////////////////
size_t OgreRenderImage::getHeight() const
{
	return m_texture->getHeight();
}
//////////////////////////////////////////////////////////////////////////
Ogre::Texture * OgreRenderImage::getTexture()
{
	return m_texture.getPointer();
}

