#	pragma once
#	include "Interface/RenderSystemInterface.h"

#	include <SDL.h>
#	include "SDL_opengl.h"

#	include <string>
#	include <map>

class OGLTexture : public Menge::RenderImageInterface
{
public:
	OGLTexture();
	~OGLTexture();
	OGLTexture(const Menge::String& _name, std::size_t _width, std::size_t _height );
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
	GLint m_glInternalFormat;
	GLenum m_glPixelFormat;
	GLenum m_glPixelType;
	unsigned char * m_buffer;

	GLuint m_texture;
	int m_ref;
	std::size_t m_width;
	std::size_t m_height;
	Menge::PixelFormat m_format;
	
	Menge::String m_name;
};