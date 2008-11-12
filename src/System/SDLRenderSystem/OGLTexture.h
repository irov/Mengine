#	pragma once

#	include "Interface/RenderSystemInterface.h"

#	include "PBO.h"
#	include <string>
#	include <map>

class OGLTexture : public Menge::RenderImageInterface
{
public:
	OGLTexture();
	OGLTexture( GLuint _id, const Menge::String & name, int width, int height );
	~OGLTexture();

public:
	GLuint getGLTexture() const;
	void load( std::size_t _width, std::size_t _heigth, const Menge::TextureDesc & _desc );
	int incRef() { return ++m_ref; }
	int decRef() { return --m_ref; }
	const float* getUVMask() const;

public:
	std::size_t getWidth() const override;
	std::size_t getHeight() const  override;
	const Menge::String & getDescription() const  override;
	unsigned char * lock( int* _pitch, bool _readOnly = true )  override;
	void unlock() override;
	Menge::PixelFormat getPixelFormat() override;

private:

	PixelBufferObject	m_PBO;

	GLuint m_texture;
	std::size_t m_width;
	std::size_t m_height;
	std::size_t m_image_width;
	std::size_t m_image_height;
	float m_uvMask[2];

	GLint m_glformat;
	GLenum m_glpixelType;

	int m_ref;

	Menge::PixelFormat m_format;

	Menge::String m_name;

	bool m_useFBO;
};