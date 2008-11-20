#	include "OGLTexture.h"
#	include <assert.h>

//////////////////////////////////////////////////////////////////////////
OGLTexture::OGLTexture()
: m_texture( 0 )
, m_bufferID( 0 )
, m_width( 0 )
, m_height( 0 )
, m_glformat( GL_BGRA )
, m_ref( 0 )
, m_format( Menge::PF_A8R8G8B8 )
, m_glpixelType( GL_UNSIGNED_BYTE )
{
}
//////////////////////////////////////////////////////////////////////////
OGLTexture::~OGLTexture()
{
	if( m_texture != 0 )
	{
		glDeleteTextures( 1, &m_texture );
	}
	if( m_bufferID != 0 )
	{
		glDeleteBuffers( 1, &m_bufferID );
	}
}
//////////////////////////////////////////////////////////////////////////
void OGLTexture::load( std::size_t _width, std::size_t _heigth, const Menge::TextureDesc & _desc )
{
	int internalFormat;

	switch (_desc.pixelFormat)
	{
	case Menge::PF_A1R5G5B5:
		m_glformat = GL_BGRA;
		m_glpixelType = GL_UNSIGNED_SHORT_1_5_5_5_REV;
		internalFormat = 3;
		break;
	case Menge::PF_R5G6B5:
		m_glformat=GL_BGR;
		m_glpixelType=GL_UNSIGNED_SHORT_5_6_5_REV;
		internalFormat = 3;
		break;
	case Menge::PF_R8G8B8:
		m_glformat=GL_BGR;
		m_glpixelType=GL_UNSIGNED_BYTE;
		internalFormat = GL_RGB;
		break;
	case Menge::PF_A8R8G8B8:
		m_glformat=GL_BGRA;
		m_glpixelType=GL_UNSIGNED_INT_8_8_8_8_REV;
		internalFormat = GL_RGBA8;		
		break;
	default:
		assert(!"Unsupported texture format");
		break;
	}

	glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );
	// generate PBO
	glGenBuffers( 1, &m_bufferID );
	glBindBuffer( GL_PIXEL_UNPACK_BUFFER_ARB, m_bufferID );
	// load data into pbo
	glBufferData( GL_PIXEL_UNPACK_BUFFER_ARB, _desc.size, _desc.buffer, GL_STREAM_COPY );
	
	// generate Texture
	glGenTextures( 1, &m_texture );

	glBindTexture( GL_TEXTURE_2D, m_texture );

	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0 );
	glTexParameteri( GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_FALSE );
	
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
	
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );

	// send pbo data to texture unit
	glTexImage2D( GL_TEXTURE_2D, 0, internalFormat, _desc.width, _desc.height, 0, m_glformat, m_glpixelType, NULL );
	/*printf( "Send format: %x\n", internalFormat );
	GLint format;
	glGetTexLevelParameteriv( GL_TEXTURE_2D, 0, GL_TEXTURE_INTERNAL_FORMAT, &format );
	printf( "Recieve format: %x\n", format );*/

	glBindBuffer( GL_PIXEL_UNPACK_BUFFER_ARB, 0 );
	glBindTexture( GL_TEXTURE_2D, 0 );

	glPixelStorei( GL_UNPACK_ALIGNMENT, 4 );
	/*if( glGetError() != GL_NO_ERROR )
	{
		printf( "Problems creating texture\n" );
	}*/

	m_format = (Menge::PixelFormat)_desc.pixelFormat;
	m_name = _desc.name;
	m_width = _desc.width;
	m_height = _desc.height;
	m_image_width = _width;
	m_image_height = _heigth;
	m_uvMask[0] = static_cast<float>( m_image_width ) / m_width;
	m_uvMask[1] = static_cast<float>( m_image_height ) / m_height;
	m_pitch = _desc.size / _desc.height;
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
	glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );
	glPixelStorei( GL_UNPACK_ROW_LENGTH, m_pitch );

	glBindBuffer( GL_PIXEL_UNPACK_BUFFER_ARB, m_bufferID );
	glBindTexture( GL_TEXTURE_2D, m_texture );
	
	GLenum flag = GL_WRITE_ONLY;
	if( _readOnly == true )
	{
		flag = GL_READ_ONLY;
	}
	GLvoid* data = glMapBuffer( GL_PIXEL_UNPACK_BUFFER_ARB, flag );
	
	glBindBuffer( GL_PIXEL_UNPACK_BUFFER_ARB, 0 );
	glBindTexture( GL_TEXTURE_2D, 0 );
	
	*_pitch = m_pitch;

	glPixelStorei( GL_UNPACK_ALIGNMENT, 4 );
	glPixelStorei( GL_UNPACK_ROW_LENGTH, 0 );

	return static_cast<unsigned char*>( data );
}
//////////////////////////////////////////////////////////////////////////
void OGLTexture::unlock()
{
	glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );
	glBindBuffer( GL_PIXEL_UNPACK_BUFFER_ARB, m_bufferID );
	glBindTexture( GL_TEXTURE_2D, m_texture );
	
	glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER_ARB);
	glTexSubImage2D( GL_TEXTURE_2D, 0, 0, 0, m_width, m_height, m_glformat, m_glpixelType, NULL );
	
	glBindBuffer( GL_PIXEL_UNPACK_BUFFER_ARB, 0 );
	glBindTexture( GL_TEXTURE_2D, 0 );
	glPixelStorei( GL_UNPACK_ALIGNMENT, 4 );
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