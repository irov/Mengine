#	pragma once
#	include "Interface/RenderSystemInterface.h"

//#	include "SDL_opengl.h"

//#	include "OGLRenderSystem.h"

#	include "PBO.h"
//#include <GL/gl.h>
//#include <GL/glext.h>
//#include <GL/glut.h>
//#include <stdio.h>


#	include <string>
#	include <map>

class OGLTexture : public Menge::RenderImageInterface
{
public:
	OGLTexture(GLint _textureType);
	OGLTexture(const Menge::String& _name, std::size_t _width, std::size_t _height, GLint _textureType );

	~OGLTexture();
public:
	void load( const Menge::TextureDesc & _desc );
	std::size_t getWidth() const override;
	std::size_t getHeight() const  override;
	const Menge::String & getDescription() const  override;
	unsigned char* lock( int* _pitch, bool _readOnly = true )  override;
	void unlock() override;
	Menge::PixelFormat getPixelFormat() override;
public:
	int incRef() { return ++m_ref; }
	int decRef() { return --m_ref; }
public:
	GLuint getGLTexture() const;

private:

	CPBO	m_PBO;

	GLint m_textureType;
	//GLuint m_bufferId;
	GLint m_glInternalFormat;
	GLenum m_glPixelFormat;
	GLenum m_glPixelType;

	GLuint m_texture;
	int m_ref;
	std::size_t m_width;
	std::size_t m_height;
	Menge::PixelFormat m_format;

	Menge::String m_name;
};