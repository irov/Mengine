#	pragma once

#	include "Interface/RenderSystemInterface.h"
#	include "OGLRenderSystem.h"

class OGLTexture : public Menge::RenderImageInterface
{
public:
	OGLTexture();
	virtual ~OGLTexture();

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
	virtual unsigned char * lock( int* _pitch, bool _readOnly = true )  override;
	virtual void unlock() override;
	Menge::PixelFormat getPixelFormat() override;

protected:
	GLuint m_bufferID;
	GLuint m_texture;
	std::size_t m_width;
	std::size_t m_height;
	std::size_t m_image_width;
	std::size_t m_image_height;
	int m_pitch;
	float m_uvMask[2];

	GLint m_glformat;
	GLenum m_glpixelType;
	int m_ref;

	Menge::PixelFormat m_format;
	Menge::String m_name;
};