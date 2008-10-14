#	include "SDLTexture.h"
#	include <assert.h>

SDLTexture::SDLTexture()
: m_texture(0)
, m_width(0)
, m_height(0)
, m_glInternalFormat(GL_RGBA)
, m_glPixelFormat(GL_BGRA_EXT)
{}

SDLTexture::SDLTexture(const Menge::String& _name, std::size_t _width, std::size_t _height )
: m_name(_name)
, m_width(_width)
, m_height(_height)
, m_glInternalFormat(GL_RGBA)
, m_glPixelFormat(GL_BGRA_EXT)
{
	//
}

SDLTexture::~SDLTexture()
{
	glDeleteTextures( 1, &m_texture );
}

GLuint SDLTexture::getGLTexture() const
{
	return m_texture;
}

void SDLTexture::load( const Menge::TextureDesc & _desc )
{
	int ncomponents = 4;

	switch (_desc.pixelFormat)
	{
	case Menge::PF_A1R5G5B5:
		m_format = Menge::PF_A1R5G5B5;
		m_glInternalFormat=GL_BGRA;
		m_glPixelFormat=GL_BGRA_EXT;
		m_glPixelType=GL_UNSIGNED_SHORT_1_5_5_5_REV;
		ncomponents = 3;
		break;
	case Menge::PF_R5G6B5:
		m_format = Menge::PF_R5G6B5;
		m_glInternalFormat=GL_BGR;
		m_glPixelFormat=GL_BGR;
		m_glPixelType=GL_UNSIGNED_SHORT_5_6_5_REV;
		ncomponents = 3;
		break;
	case Menge::PF_R8G8B8:
		m_format = Menge::PF_R8G8B8;
		m_glInternalFormat=GL_BGR;
		m_glPixelFormat=GL_BGR;
		m_glPixelType=GL_UNSIGNED_BYTE;
		ncomponents = 3;
		break;
	case Menge::PF_A8R8G8B8:
		m_format = Menge::PF_A8R8G8B8;
		m_glInternalFormat=GL_BGRA;
		m_glPixelFormat=GL_BGRA_EXT;
		m_glPixelType=GL_UNSIGNED_BYTE;
		ncomponents = 4;		
		break;
	default:
		assert(!"Unsupported texture format");
		break;
	}

	glGenTextures(1,&m_texture);
	glBindTexture(GL_TEXTURE_2D,m_texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

//	gluBuild2DMipmaps(GL_TEXTURE_2D,ncomponents,_desc.width,_desc.height,m_glInternalFormat,m_glPixelType,_desc.buffer);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, _desc.width,_desc.height, 0, m_glInternalFormat,m_glPixelType,_desc.buffer);

	m_name = _desc.name;
	m_width = _desc.width;
	m_height = _desc.height;
	m_buffer = static_cast<unsigned char*>(_desc.buffer);
}

std::size_t SDLTexture::getWidth() const
{
	return m_width;
}

std::size_t SDLTexture::getHeight() const
{
	return m_height;
}

const Menge::String & SDLTexture::getDescription() const
{
	return m_name;
}

unsigned char* SDLTexture::lock( int* _pitch, bool _readOnly )
{
	return m_buffer;
}

void SDLTexture::unlock()
{
	glBindTexture(GL_TEXTURE_2D, m_texture);
	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_width, 
		m_height, 0, GL_BGRA_EXT , GL_UNSIGNED_BYTE, m_buffer);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

Menge::PixelFormat SDLTexture::getPixelFormat() 
{
	return m_format;
}