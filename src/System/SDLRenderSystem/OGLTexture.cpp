#	include "OGLTexture.h"
#	include <assert.h>

//////////////////////////////////////////////////////////////////////////
OGLTexture::OGLTexture()
: m_texture(0)
, m_width(0)
, m_height(0)
, m_glformat(GL_BGRA)
, m_ref(0)
, m_format(Menge::PF_A8R8G8B8)
, m_glpixelType(GL_UNSIGNED_BYTE)
, m_useFBO(false)
{
}
//////////////////////////////////////////////////////////////////////////
OGLTexture::OGLTexture( GLuint _id, const Menge::String & name, int width, int height )
: m_texture(_id)
, m_glformat(GL_BGRA)
, m_ref(0)
, m_format(Menge::PF_A8R8G8B8)
, m_glpixelType(GL_UNSIGNED_BYTE)
, m_useFBO(false)
{
	int size = width * height * 4;

	m_PBO.init(size);
	m_PBO.unbind();

	m_name = name;
	m_width = m_image_width = width;
	m_height = m_image_height = height;
	m_uvMask[0] = m_uvMask[1] = 1.0f;
}
//////////////////////////////////////////////////////////////////////////
OGLTexture::~OGLTexture()
{
	m_PBO.done();
	glDeleteTextures( 1, &m_texture );
}
//////////////////////////////////////////////////////////////////////////
void OGLTexture::load( std::size_t _width, std::size_t _heigth, const Menge::TextureDesc & _desc )
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

	glBindTexture(GL_TEXTURE_2D,m_texture);

	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );

	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );

	glTexImage2D( GL_TEXTURE_2D, 0, ncomponents, _desc.width, _desc.height, 0, m_glformat, m_glpixelType, _desc.buffer );

	int size = _desc.width * _desc.height * ncomponents;

	m_PBO.init( size );
	m_PBO.unbind();

	GLenum error = glGetError();

	if( error != GL_NO_ERROR ) 
	{
		printf( "Texture error\n" );
	}

	m_name = _desc.name;
	m_width = _desc.width;
	m_height = _desc.height;
	m_image_width = _width;
	m_image_height = _heigth;
	m_uvMask[0] = static_cast<float>( m_image_width ) / m_width;
	m_uvMask[1] = static_cast<float>( m_image_height ) / m_height;
}
//////////////////////////////////////////////////////////////////////////
GLuint OGLTexture::getGLTexture() const
{
	return m_texture;
}
///////////////////////////////////////////////////////////////////////////
std::size_t OGLTexture::getWidth() const
{
	return m_image_width;
}
//////////////////////////////////////////////////////////////////////////
std::size_t OGLTexture::getHeight() const
{
	return m_image_height;
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

	glBindTexture( GL_TEXTURE_2D, m_texture );

	return m_PBO.map(!_readOnly);
}
//////////////////////////////////////////////////////////////////////////
void OGLTexture::unlock()
{
	// unmap the buffer from the CPU space so it can DMA
	m_PBO.unmap();
	// bind us to the right texture object
	glBindTexture( GL_TEXTURE_2D, m_texture );
	// kick off the DMA
	glTexSubImage2D( GL_TEXTURE_2D, 0, 0, 0, m_width, m_height, m_glformat, m_glpixelType, NULL );
	// unbind the PBO 
	m_PBO.unbind();
}
//////////////////////////////////////////////////////////////////////////
Menge::PixelFormat OGLTexture::getPixelFormat() 
{
	return m_format;
}
//////////////////////////////////////////////////////////////////////////
const float* OGLTexture::getUVMask() const
{
	return m_uvMask;
}
//////////////////////////////////////////////////////////////////////////