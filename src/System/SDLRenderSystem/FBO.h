#	pragma once
#	include "OGLRenderSystem.h"

class	FrameBufferObject
{
public:
	enum								// flags for depth and stencil buffers
	{
		depth16 = 1,					// 16-bit depth buffer
		depth24 = 2,					// 24-bit depth buffer
		depth32 = 4,					// 32-bit depth buffer
		
		stencil1  = 16,					// 1-bit stencil buffer
		stencil4  = 32,					// 4-bit stencil buffer
		stencil8  = 64,					// 8-bit stencil buffer
		stencil16 = 128					// 16-bit stencil buffer
	};
	
	enum								// filter modes
	{
		filterNearest = 0,
		filterLinear  = 1,
		filterMipmap  = 2
	};
public:
	FrameBufferObject(int _width, int _height, int _flags = 0);
	~FrameBufferObject();

public:
	int	getWidth() const;
	int	getHeight() const;
	bool hasStencil() const;
	bool hasDepth() const;
	unsigned int getColorBuffer(int no = 0) const;
	unsigned int getDepthBuffer() const;

	bool isOk() const;
	bool create();
	bool bind();
	bool bindFace(int no);
	bool unbind();
	
	bool attachColorTexture ( GLenum target, unsigned texId, int no = 0 );
	bool attachDepthTexture ( GLenum target, unsigned texId );
	
	bool detachColorTexture ( GLenum target );
	bool detachDepthTexture ( GLenum target );
	
	unsigned int createColorTexture(GLenum format = GL_RGBA, GLenum internalFormat = GL_RGBA8, 
	                                     GLenum clamp = GL_REPEAT, int filter = filterLinear);
										 
	unsigned int createColorRectTexture(GLenum format = GL_RGBA, GLenum internalFormat = GL_RGBA8);
	
	unsigned int createCubemapTexture(GLenum format = GL_RGBA, GLenum internalFormat = GL_RGBA8, 
	                                     GLenum clamp = GL_CLAMP_TO_EDGE, int filter = filterLinear);
										 
	void	buildMipmaps (GLenum target = GL_TEXTURE_2D) const;
	
	
	
	static	int		maxColorAttachemnts();
	static	int		maxSize();

private:
	int		flags;
	int		width;
	int		height;
	GLuint	frameBuffer;					// id of framebuffer object
	GLuint	colorBuffer[8];				// texture id or buffer id
	GLuint	depthBuffer;
	GLuint	stencilBuffer;	
	GLint	saveViewport[4];
};