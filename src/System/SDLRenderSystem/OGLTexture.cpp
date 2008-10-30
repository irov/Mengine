#	include "OGLTexture.h"
#	include <assert.h>

//////////////////////////////////////////////////////////////////////////
OGLTexture::OGLTexture(GLint _textureType)
: m_texture(0)
, m_width(0)
, m_height(0)
, m_glInternalFormat(GL_RGBA)
, m_glPixelFormat(GL_BGRA)
, m_ref(0)
, m_format(Menge::PF_A8R8G8B8)
, m_glPixelType(GL_UNSIGNED_BYTE)
, m_textureType(_textureType)
{
	/*glGenBuffers(1, &m_bufferId);
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER_ARB, m_bufferId);
	glBufferData(GL_PIXEL_UNPACK_BUFFER_ARB, m_width*m_height*4*sizeof(unsigned char), NULL, GL_STREAM_DRAW);
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER_ARB, 0); // unbind 

	glGenTextures(1, &m_texture);
	glBindTexture(m_textureType, m_texture);

	//glTexParameteri(m_textureType, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(m_textureType, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(m_textureType, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);*/
}
//////////////////////////////////////////////////////////////////////////
OGLTexture::OGLTexture(const Menge::String& _name, std::size_t _width, std::size_t _height, GLint _textureType)
: m_name(_name)
, m_width(_width)
, m_height(_height)
, m_glInternalFormat(GL_RGBA)
, m_glPixelFormat(GL_BGRA)
, m_ref(0)
, m_format(Menge::PF_A8R8G8B8)
, m_glPixelType(GL_UNSIGNED_BYTE)
//, m_bufferId(0)
, m_textureType(_textureType)
{
	int size = m_width*m_height*4*sizeof(unsigned char);
	m_PBO.Init(size);
	m_PBO.UnBind();

	glGenTextures(1, &m_texture);
	glBindTexture(m_textureType, m_texture);

	//glTexParameteri(m_textureType, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	//glTexParameteri(m_textureType, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTexImage2D(m_textureType, 0, GL_RGBA8, m_width, m_height, 0, m_glInternalFormat,m_glPixelType,0);

}
//////////////////////////////////////////////////////////////////////////
OGLTexture::~OGLTexture()
{
	m_PBO.Done();
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
		m_glPixelFormat=GL_BGRA;
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
		m_glPixelFormat=GL_BGRA;
		m_glPixelType=GL_UNSIGNED_BYTE;
		ncomponents = 4;		
		break;
	default:
		assert(!"Unsupported texture format");
		break;
	}


	int size = _desc.width * _desc.height * ncomponents * sizeof(unsigned char);
	m_PBO.Init(size);
	m_PBO.UnBind();

	glGenTextures(1,&m_texture);
	glBindTexture(m_textureType,m_texture);

	//glTexParameteri(m_textureType, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(m_textureType, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(m_textureType, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTexParameteri(m_textureType, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(m_textureType, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glTexImage2D(m_textureType, 0, GL_RGBA8, _desc.width,_desc.height, 0, m_glInternalFormat,m_glPixelType,_desc.buffer);

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
/*	glBindTexture(m_textureType,m_texture);

	glBindBuffer( GL_PIXEL_UNPACK_BUFFER_ARB, m_bufferId );

	if( _readOnly )
	{
		glBufferData( GL_PIXEL_UNPACK_BUFFER_ARB, 0, NULL, 0 );
	}

	glBufferData(GL_PIXEL_UNPACK_BUFFER_ARB, m_width*m_height*4*sizeof(unsigned char), NULL, GL_STREAM_DRAW);

	void * pixels = glMapBuffer( GL_PIXEL_UNPACK_BUFFER_ARB, GL_WRITE_ONLY );

	*_pitch = m_width * 4;

	return static_cast<unsigned char*>(pixels);*/

//	return NULL;

	*_pitch = m_width * 4;
	return m_PBO.Map(!_readOnly);
}
//////////////////////////////////////////////////////////////////////////
void OGLTexture::unlock()
{
	m_PBO.Unmap();
	
	glBindTexture(m_textureType,m_texture);
	glTexSubImage2D(m_textureType, 0, GL_RGBA8, m_width,m_height, 0, m_glInternalFormat,m_glPixelType,0);

	m_PBO.UnBind();

/*	glBindBufferARB( GL_PIXEL_UNPACK_BUFFER_ARB, m_bufferId );
	glUnmapBufferARB( GL_PIXEL_UNPACK_BUFFER_ARB );

	glBindTexture(m_textureType,m_texture);
	glTexSubImage2D(m_textureType, 0, GL_RGBA8, m_width,m_height, 0, m_glInternalFormat,m_glPixelType,0);
	glBindBufferARB( GL_PIXEL_UNPACK_BUFFER_ARB, 0 );*/
}
//////////////////////////////////////////////////////////////////////////
Menge::PixelFormat OGLTexture::getPixelFormat() 
{
	return m_format;
}
//////////////////////////////////////////////////////////////////////////