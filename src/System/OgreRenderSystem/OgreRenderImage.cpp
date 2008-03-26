#	include "OgreRenderImage.h"
#	include "OgreImageCodec.h"

//////////////////////////////////////////////////////////////////////////
OgreRenderImage::OgreRenderImage()
: m_description( "" )
, m_texture()
{
}
//////////////////////////////////////////////////////////////////////////
OgreRenderImage::OgreRenderImage( const char* _name, unsigned int _width, unsigned int _height, Ogre::TextureUsage _usage, Ogre::ManualResourceLoader* _loader  )
: m_description( _name )
{
	m_texture = Ogre::TextureManager::getSingleton().createManual( _name, "Default", Ogre::TEX_TYPE_2D,
																	_width, _height, 0, 0, Ogre::PF_X8R8G8B8, 
																	_usage, _loader );
}
//////////////////////////////////////////////////////////////////////////
OgreRenderImage::OgreRenderImage( const TextureDesc& _desc )
: m_description( _desc.name )
{
	Ogre::Image image;

	Ogre::DataStreamPtr data( new Ogre::MemoryDataStream( _desc.buffer, _desc.size ) );
	
	Ogre::String::size_type index = m_description.find_last_of(".");
	std::string ext = m_description.substr( index + 1, m_description.size() );

	image.load( data, ext );
	//image.FILTER_BILINEAR


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
float OgreRenderImage::getWidth() const
{ 
	return m_texture->getWidth();
}
//////////////////////////////////////////////////////////////////////////
float OgreRenderImage::getHeight() const
{
	return m_texture->getHeight();
}
//////////////////////////////////////////////////////////////////////////
void OgreRenderImage::writeToFile( const char* _filename )
{
	// copyToMemory
	Ogre::ImageCodec::ImageData *imgData = new Ogre::ImageCodec::ImageData();
        
    imgData->width = m_texture->getWidth();
    imgData->height = m_texture->getHeight();
	imgData->depth = 1;
	imgData->format = Ogre::PF_BYTE_RGBA;
	size_t size = imgData->width * imgData->height * 4;	// 4 bytes per pixel in RGBA

    // Allocate buffer 
    unsigned char* pBuffer = new unsigned char[size];

    // Read pixels
    m_texture->getBuffer()->blitToMemory(
		Ogre::Box(0, 0, imgData->width, imgData->height), 
		Ogre::PixelBox(imgData->width, imgData->height, 1, imgData->format, pBuffer)	);

	// Wrap buffer in a chunk
	Ogre::MemoryDataStreamPtr stream(new Ogre::MemoryDataStream( pBuffer, size, false ) );

    // Get codec 
	Ogre::String filename(_filename);
	Ogre::String::size_type pos = filename.find_last_of(".");
	Ogre::String extension;
	if( pos == Ogre::String::npos )
	{
		// TODO: invalid extension exception
	}
	while( pos != filename.length() - 1 )
	{
		extension += filename[++pos];
	}

    // Get the codec
	Ogre::Codec * pCodec = Ogre::Codec::getCodec(extension);

    // Write out
	Ogre::Codec::CodecDataPtr codecDataPtr(imgData);
    pCodec->codeToFile(stream, filename, codecDataPtr);

	delete [] pBuffer;
}
//////////////////////////////////////////////////////////////////////////
Ogre::Texture * OgreRenderImage::getTexture() const
{
	return m_texture.getPointer();
}

