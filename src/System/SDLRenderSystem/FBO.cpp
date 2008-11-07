#include	"FBO.h"
#include	<stdlib.h>

FrameBufferObject::FrameBufferObject(GLint _textureType, int _width, int _height, int _flags)
: m_textureType(_textureType)
{
	width         = _width;
	height        = _height;
	flags         = _flags;
	frameBuffer   = 0;
	depthBuffer   = 0;
	stencilBuffer = 0;
	
	for ( int i = 0; i < 8; i++ )
	{
		colorBuffer[i] = 0;
	}
}

FrameBufferObject::~FrameBufferObject()
{
	if (depthBuffer != 0)
	{
		glDeleteRenderbuffersEXT(1,&depthBuffer);
	}

	if (stencilBuffer != 0)
	{
		glDeleteRenderbuffersEXT(1,&stencilBuffer);
	}

	if (frameBuffer != 0)
	{
		glDeleteFramebuffersEXT(1,&frameBuffer);
	}
}

int	FrameBufferObject::getWidth() const
{
	return width;
}

int	FrameBufferObject::getHeight() const
{
	return height;
}

bool FrameBufferObject::hasStencil() const
{
	return stencilBuffer != 0;
}

bool FrameBufferObject::hasDepth() const
{
	return depthBuffer != 0;
}

unsigned int FrameBufferObject::getColorBuffer(int _no) const
{
	return colorBuffer[_no];
}

unsigned int FrameBufferObject::getDepthBuffer() const
{
	return depthBuffer;
}

bool FrameBufferObject::create()
{
	if ( width <= 0 || height <= 0 )
	{
		return false;
	}

	glGenFramebuffersEXT ( 1, &frameBuffer );
	glBindFramebufferEXT ( GL_FRAMEBUFFER_EXT, frameBuffer );

	//checkFramebufferStatus();

	int	depthFormat   = 0;
	int stencilFormat = 0;

	if ( flags & depth16 )
		depthFormat = GL_DEPTH_COMPONENT16_ARB;
	else
	if ( flags & depth24 )
		depthFormat = GL_DEPTH_COMPONENT24_ARB;
	else
	if ( flags & depth32 )
		depthFormat = GL_DEPTH_COMPONENT32_ARB;

	if ( flags & stencil1 )
		stencilFormat = GL_STENCIL_INDEX1_EXT;
	else
	if ( flags & stencil4 )
		stencilFormat = GL_STENCIL_INDEX4_EXT;
	else
	if ( flags & stencil8 )
		stencilFormat = GL_STENCIL_INDEX8_EXT;
	else
	if ( flags & stencil16 )
		stencilFormat = GL_STENCIL_INDEX16_EXT;

	if (depthFormat != 0)
	{
		glGenRenderbuffersEXT(1, &depthBuffer);
		glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, depthBuffer);
		glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, depthFormat, width, height);
		glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT,  GL_DEPTH_ATTACHMENT_EXT,
		                               GL_RENDERBUFFER_EXT, depthBuffer);
	}

	if (stencilFormat != 0)
	{
		glGenRenderbuffersEXT(1, &stencilBuffer);
		glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, stencilBuffer);
		glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, stencilFormat, width, height);
		glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT,  GL_STENCIL_ATTACHMENT_EXT,
		                               GL_RENDERBUFFER_EXT, stencilBuffer);
	}

	GLenum status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

	return status == GL_FRAMEBUFFER_COMPLETE_EXT;
}

bool FrameBufferObject::isOk() const
{
	GLint currentFb;

	glGetIntegerv(GL_FRAMEBUFFER_BINDING_EXT, &currentFb);

	if (currentFb != frameBuffer)
	{
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, frameBuffer);
		glReadBuffer(GL_COLOR_ATTACHMENT0_EXT);
	}

	int	 status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);

	if (currentFb != frameBuffer)
	{
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, currentFb);
	}

	return 	status == GL_FRAMEBUFFER_COMPLETE_EXT;
}

bool FrameBufferObject::bind()
{
	if (frameBuffer == 0)
	{
		return false;
	}

	glFlush();
								
	glGetIntegerv(GL_VIEWPORT, saveViewport);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, frameBuffer);
	glReadBuffer(GL_COLOR_ATTACHMENT0_EXT);
	glDrawBuffer(GL_COLOR_ATTACHMENT0_EXT);
	glViewport(0, 0, getWidth(), getHeight());

	return true;
}

bool FrameBufferObject::bindFace(int _face)
{
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB + _face,colorBuffer[0],0);
	return bind();
}

bool FrameBufferObject::unbind()
{
	if ( frameBuffer == 0 )
	{
		return false;
	}

	glFlush();
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
	glViewport(saveViewport[0], saveViewport[1], saveViewport[2], saveViewport[3]);

	return true;
}

bool FrameBufferObject::attachColorTexture(GLenum _target, unsigned texId, int no)
{
	if (frameBuffer == 0)
	{
		return false;
	}

	if (_target != m_textureType && _target != GL_TEXTURE_RECTANGLE_ARB && (_target < GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB || _target > GL_TEXTURE_CUBE_MAP_NEGATIVE_Z_ARB))
	{
		return false;
	}

	glBindTexture(_target, colorBuffer[no] = texId);
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT + no, _target, texId, 0);

	checkFramebufferStatus();

	return true;
}

bool FrameBufferObject::attachDepthTexture(GLenum _target, unsigned int _texId)
{
	if (frameBuffer == 0)
	{
		return false;
	}

	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, _target, depthBuffer = _texId, 0);

	return true;
}

unsigned int FrameBufferObject::createColorTexture(GLenum format, GLenum internalFormat, GLenum clamp, int filter )
{
	GLuint	tex;

	glGenTextures(1, &tex);
    glBindTexture(m_textureType, tex);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);                         // set 1-byte alignment
    glTexImage2D(m_textureType, 0, internalFormat, getWidth(), getHeight(), 0,
                      format, GL_UNSIGNED_BYTE, NULL);

	if (filter == filterNearest)
	{
		glTexParameteri(m_textureType, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(m_textureType, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	}
	else
	if (filter == filterLinear)
	{
		glTexParameteri(m_textureType, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(m_textureType, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	}
	else
	if (filter == filterMipmap )
	{
		glTexParameteri(m_textureType, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(m_textureType, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	}
	
    glTexParameteri(m_textureType, GL_TEXTURE_WRAP_S, clamp);
    glTexParameteri(m_textureType, GL_TEXTURE_WRAP_T, clamp);

    return tex;
}

unsigned int FrameBufferObject::createCubemapTexture(GLenum format, GLenum internalFormat, GLenum clamp, int filter)
{
	if (getWidth() != getHeight ())
	{
		return 0;
	}
		
	GLuint	tex;

	glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_CUBE_MAP_ARB, tex);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);                        
    glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_WRAP_S, clamp);
    glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_WRAP_T, clamp);
    glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_WRAP_R, clamp);
	
    for ( int side = 0; side < 6; side++ )
	{
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB + side, 0, internalFormat,
                       getWidth (), getWidth (), 0, format, GL_UNSIGNED_BYTE, NULL );
	}
	
	if (filter == filterNearest)
	{
		glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
		glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
	}
	else
	if (filter == filterLinear)
	{
		glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
		glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	}
	else
	if (filter == filterMipmap)
	{
		glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
		glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
	}
	
    return tex;
}

unsigned int FrameBufferObject::createColorRectTexture(GLenum format, GLenum internalFormat)
{
	GLuint	tex;

	glGenTextures(1, &tex);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);                        
    glBindTexture(GL_TEXTURE_RECTANGLE_ARB, tex);

    glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, internalFormat,getWidth(), getHeight(), 0,
                      format, GL_UNSIGNED_BYTE, NULL);

	glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_T, GL_CLAMP);

    return tex;
}

int	FrameBufferObject::maxColorAttachemnts()
{
    GLint n = 0;
    glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS_EXT, &n);
	return n;
}

int	FrameBufferObject::maxSize()
{
    GLint sz = 0;
    glGetIntegerv(GL_MAX_RENDERBUFFER_SIZE_EXT, &sz);
	return sz;
}

bool FrameBufferObject::detachColorTexture(GLenum _target)
{
	return attachColorTexture(_target, 0);
}

bool FrameBufferObject::detachDepthTexture(GLenum _target)
{
	return attachDepthTexture(_target, 0);
}

void FrameBufferObject::buildMipmaps(GLenum _target) const
{
	glBindTexture(_target, getColorBuffer());
	glGenerateMipmapEXT(_target);
}

GLenum FrameBufferObject::checkFramebufferStatus(bool _printMessage)
{
	GLenum error;
	GLint oldFB = 0;
	glGetIntegerv( GL_FRAMEBUFFER_BINDING_EXT, &oldFB );

	// there may be some other framebuffer currently bound...  if so, save it 
	if (oldFB != frameBuffer)
	{
		glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, frameBuffer);
	}
	
	// check the error status of this framebuffer */
	error = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);

	if (_printMessage)
	{
		switch(error)
		{
			case GL_FRAMEBUFFER_COMPLETE_EXT:
				break;
			case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT:
				printf("Error!  %d missing a required image/buffer attachment!\n", frameBuffer);
				break;
			case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT:
				printf("Error!  %d has no images/buffers attached!\n", frameBuffer);
				break;
			case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT:
				printf("Error!  %d has mismatched image/buffer dimensions!\n", frameBuffer);
				break;
			case GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT:
				printf("Error!  %d colorbuffer attachments have different types!\n", frameBuffer);
				break;
			case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT:
				printf("Error!  %d trying to draw to non-attached color buffer!\n", frameBuffer);
				break;
			case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT:
				printf("Error!  %d trying to read from a non-attached color buffer!\n", frameBuffer);
				break;
			case GL_FRAMEBUFFER_UNSUPPORTED_EXT:
				printf("Error!  %d format is not supported by current graphics card/driver!\n", frameBuffer);
				break;
			default:
				printf("*UNKNOWN ERROR* reported from glCheckFramebufferStatusEXT() for %d!\n", frameBuffer);
				break;
		}
	}

	if (oldFB != frameBuffer)
	{
		glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, oldFB );
	}

	return error;
}