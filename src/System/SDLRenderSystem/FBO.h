#	pragma once
#	include "OGLRenderSystem.h"

class	FrameBufferObject
{
public:
	enum								
	{
		depth16 = 1,					
		depth24 = 2,					
		depth32 = 4,					
		
		stencil1  = 16,					
		stencil4  = 32,					
		stencil8  = 64,					
		stencil16 = 128					
	};
	
	enum								
	{
		filterNearest = 0,
		filterLinear  = 1,
		filterMipmap  = 2
	};

public:
	FrameBufferObject( int _width, int _height, int _flags = 0 );
	~FrameBufferObject();

public:
	int	getWidth() const;
	int	getHeight() const;
	bool hasStencil() const;
	bool hasDepth() const;
	unsigned int getColorBuffer(int _no = 0) const;
	unsigned int getDepthBuffer() const;

	bool isOk() const;
	bool create();
	bool bind();
	bool bindFace(int _no);
	bool unbind();
	
	bool attachColorTexture( unsigned int _texId, int _no = 0);
	bool attachDepthTexture(GLenum _target, unsigned int _texId);
	
	bool detachColorTexture(GLenum _target);
	bool detachDepthTexture(GLenum _target);
	
	unsigned int createColorTexture(GLenum _format = GL_RGBA, GLenum _internalFormat = GL_RGBA8, 
	                                     GLenum _clamp = GL_REPEAT, int _filter = filterLinear);
										 
	unsigned int createColorRectTexture(GLenum _format = GL_RGBA, GLenum _internalFormat = GL_RGBA8);
	
	unsigned int createCubemapTexture(GLenum _format = GL_RGBA, GLenum _internalFormat = GL_RGBA8, 
	                                     GLenum _clamp = GL_CLAMP_TO_EDGE, int _filter = filterLinear);
										 
	void	buildMipmaps (GLenum _target) const;
	
	static	int maxColorAttachemnts();
	static	int	maxSize();

	GLenum  checkFramebufferStatus(bool _printMessage = true);

private:
	int		flags;
	int		width;
	int		height;
	GLuint	frameBuffer;		
	GLuint	colorBuffer[8];
	GLuint	depthBuffer;
	GLuint	stencilBuffer;
	GLint	saveViewport[4];
};