#	include "OGLTexture.h"
#	include <assert.h>

//////////////////////////////////////////////////////////////////////////
OGLTexture::OGLTexture()
: m_texture(0)
, m_width(0)
, m_height(0)
, m_glInternalFormat(GL_RGBA)
, m_glPixelFormat(GL_BGRA_EXT)
, m_ref(0)
, m_format(Menge::PF_A8R8G8B8)
, m_glPixelType(GL_UNSIGNED_BYTE)
, m_buffer(0)
{}
//////////////////////////////////////////////////////////////////////////
OGLTexture::OGLTexture(const Menge::String& _name, std::size_t _width, std::size_t _height )
: m_name(_name)
, m_width(_width)
, m_height(_height)
, m_glInternalFormat(GL_RGBA)
, m_glPixelFormat(GL_BGRA_EXT)
, m_ref(0)
, m_format(Menge::PF_A8R8G8B8)
, m_glPixelType(GL_UNSIGNED_BYTE)
, m_buffer(0)
{
	glGenTextures(1, &m_texture);
	glBindTexture(GL_TEXTURE_2D, m_texture);

	//glBindBuffer(GL_PIXEL_UNPACK_BUFFER_ARB, 0);


	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_width, m_height, 0, m_glInternalFormat,m_glPixelType,m_buffer);
//	glGenBuffers(1, &m_bufferId);

	glGenBuffers(1, &m_bufferId);
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER_EXT, m_bufferId);
	glBufferData(GL_PIXEL_UNPACK_BUFFER_EXT, m_width*m_height*4, NULL, GL_STREAM_DRAW);
	//glBindBuffer(GL_PIXEL_UNPACK_BUFFER_EXT, 0);

	//glBufferData( GL_PIXEL_UNPACK_BUFFER_ARB, 0, NULL, GL_STREAM_DRAW );
//	void* ioMem = glMapBuffer(GL_PIXEL_UNPACK_BUFFER_ARB, GL_WRITE_ONLY );

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}
//////////////////////////////////////////////////////////////////////////
OGLTexture::~OGLTexture()
{
	glDeleteTextures( 1, &m_texture );
}
//////////////////////////////////////////////////////////////////////////
GLuint OGLTexture::getGLTexture() const
{
	return m_texture;
}
//////////////////////////////////////////////////////////////////////////
void OGLTexture::load( const Menge::TextureDesc & _desc )
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

	glBindBuffer(GL_PIXEL_UNPACK_BUFFER_EXT, 0);

	glBindTexture(GL_TEXTURE_2D,m_texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, _desc.width,_desc.height, 0, m_glInternalFormat,m_glPixelType,_desc.buffer);

	glGenBuffers(1, &m_bufferId);

	m_name = _desc.name;
	m_width = _desc.width;
	m_height = _desc.height;
	m_buffer = static_cast<unsigned char*>(_desc.buffer);
}
//////////////////////////////////////////////////////////////////////////
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
unsigned char* OGLTexture::lock( int* _pitch, bool _readOnly )
{
	glBindBuffer( GL_PIXEL_UNPACK_BUFFER_EXT, m_bufferId );

	if( _readOnly )
	{
		glBufferData( GL_PIXEL_UNPACK_BUFFER_EXT, 0, NULL, 0 );
	}

	glBufferData(GL_PIXEL_UNPACK_BUFFER_EXT, m_width*m_height*4, NULL, GL_STREAM_DRAW);

	void * pixels = glMapBuffer( GL_PIXEL_UNPACK_BUFFER_ARB, GL_WRITE_ONLY );

	*_pitch = m_width * 4;

	return static_cast<unsigned char*>(pixels);
}
//////////////////////////////////////////////////////////////////////////
void OGLTexture::unlock()
{
	glBindBufferARB( GL_PIXEL_UNPACK_BUFFER_EXT, m_bufferId );
	glUnmapBufferARB( GL_PIXEL_UNPACK_BUFFER_EXT );
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_width,m_height, 0, m_glInternalFormat,m_glPixelType,0);
}
//////////////////////////////////////////////////////////////////////////
Menge::PixelFormat OGLTexture::getPixelFormat() 
{
	return m_format;
}
//////////////////////////////////////////////////////////////////////////