#	include "OGLTexture.h"
#	include <assert.h>

//////////////////////////////////////////////////////////////////////////
OGLTexture::OGLTexture(GLint _textureType)
: m_texture(0)
, m_width(0)
, m_height(0)
, m_glformat(GL_BGRA)
, m_ref(0)
, m_format(Menge::PF_A8R8G8B8)
, m_glpixelType(GL_UNSIGNED_BYTE)
, m_textureType(_textureType)
, m_useFBO(false)
{
}
//////////////////////////////////////////////////////////////////////////
OGLTexture::~OGLTexture()
{
	m_PBO.done();
	glDeleteTextures( 1, &m_texture );
}
//////////////////////////////////////////////////////////////////////////
void OGLTexture::load( const Menge::TextureDesc & _desc )
{
	int ncomponents = 4;

	switch (_desc.pixelFormat)
	{
	case Menge::PF_A1R5G5B5:
		m_glformat = GL_BGRA;
		m_glpixelType = GL_UNSIGNED_SHORT_1_5_5_5_REV;
		ncomponents = 3;
		break;
	case Menge::PF_R5G6B5:
		m_glformat=GL_BGR;
		m_glpixelType=GL_UNSIGNED_SHORT_5_6_5_REV;
		ncomponents = 3;
		break;
	case Menge::PF_R8G8B8:
		m_glformat=GL_BGR;
		m_glpixelType=GL_UNSIGNED_BYTE;
		ncomponents = 3;
		break;
	case Menge::PF_A8R8G8B8:
		m_glformat=GL_BGRA;
		m_glpixelType=GL_UNSIGNED_BYTE;
		ncomponents = 4;		
		break;
	default:
		assert(!"Unsupported texture format");
		break;
	}

	glPixelStorei(GL_UNPACK_ALIGNMENT,1);

	glGenTextures(1,&m_texture);
	// bind the empty buffer
	//glBindBuffer(GL_PIXEL_UNPACK_BUFFER_ARB, 0);

	glBindTexture(m_textureType,m_texture);

	glTexParameteri(m_textureType, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(m_textureType, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTexParameteri(m_textureType, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(m_textureType, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glTexImage2D(m_textureType, 0, ncomponents, _desc.width, _desc.height, 0, m_glformat, m_glpixelType, _desc.buffer);

	int size = _desc.width * _desc.height * ncomponents;

	m_PBO.init(size);
	m_PBO.unbind();

	GLenum error = glGetError();

	if( error != GL_NO_ERROR ) 
	{
		printf( "Texture error\n" );
	}

	m_name = _desc.name;
	m_width = _desc.width;
	m_height = _desc.height;
}
//////////////////////////////////////////////////////////////////////////
GLuint OGLTexture::getGLTexture() const
{
	return m_texture;
}
///////////////////////////////////////////////////////////////////////////
std::size_t OGLTexture::getWidth() const
{
	return m_width;
}
//////////////////////////////////////////////////////////////////////////
std::size_t OGLTexture::getHeight() const
{
	return m_height;
}
//////////////////////////////////////////////////////////////////////////
const Menge::String & OGLTexture::getDescription() const
{
	return m_name;
}
//////////////////////////////////////////////////////////////////////////
unsigned char * OGLTexture::lock( int* _pitch, bool _readOnly )
{
	*_pitch = m_width * 4;

	glBindTexture(GL_TEXTURE_2D, m_texture);

	return m_PBO.map(!_readOnly);
}
//////////////////////////////////////////////////////////////////////////
void OGLTexture::unlock()
{
	// unmap the buffer from the CPU space so it can DMA
	m_PBO.unmap();
	// bind us to the right texture object
	glBindTexture(m_textureType,m_texture);
	// kick off the DMA
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_width, m_height, m_glformat, m_glpixelType, NULL);
	// unbind the PBO 
	m_PBO.unbind();
}
//////////////////////////////////////////////////////////////////////////
Menge::PixelFormat OGLTexture::getPixelFormat() 
{
	return m_format;
}
//////////////////////////////////////////////////////////////////////////