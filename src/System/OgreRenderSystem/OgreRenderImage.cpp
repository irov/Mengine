#	include "OgreRenderImage.h"

//////////////////////////////////////////////////////////////////////////
OgreRenderImage::OgreRenderImage( const char* _name, unsigned int _width, unsigned int _height )
: m_description( _name )
{
	m_texture = Ogre::TextureManager::getSingleton().createManual( _name, "Default", Ogre::TEX_TYPE_2D,
																	_width, _height, 0, 0, Ogre::PF_X8R8G8B8, 
																	Ogre::TU_DEFAULT);
}
//////////////////////////////////////////////////////////////////////////
OgreRenderImage::OgreRenderImage( const TextureDesc& _desc )
: m_description( _desc.name )
{
	Ogre::Image image;

	Ogre::DataStreamPtr data( new Ogre::MemoryDataStream( _desc.buffer, _desc.size ) );
	
	size_t index = m_description.find_last_of(".");
	std::string ext = m_description.substr( index + 1, m_description.size() );

	image.load( data, ext );

	m_texture = Ogre::TextureManager::getSingletonPtr()
		->loadImage( m_description, "Default", image, Ogre::TEX_TYPE_2D, 0 );
}
//////////////////////////////////////////////////////////////////////////
OgreRenderImage::~OgreRenderImage()
{
	Ogre::TextureManager::getSingletonPtr()
		->remove( m_description );
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
Ogre::Texture * OgreRenderImage::getTexture() const
{
	return m_texture.getPointer();
}

