#	pragma once

#	include "Interface/RenderSystemInterface.h"

#if WIN32
#   include <Windows.h>
#   include <GLee.h>
#   include <GL/GLu.h>

#   include "OGLWindowContext.h"
#elif (TARGET_OS_MAC && !TARGET_OS_IPHONE)
#   include "OpenGL/gl.h"
#elif TARGET_MARMALADE
#   include "IwGL.h"
#   include "s3e.h"
#else
#   include <OpenGLES/ES1/gl.h>
#   include <OpenGLES/ES1/glext.h>
#endif

namespace Menge
{
	class OGLTexture 
		: public RenderImageInterface
	{
	public:
		OGLTexture( GLuint _uid, size_t _width, size_t _height, size_t _channels, int _pitch, PixelFormat _pixelFormat, GLint _internalFormat, GLenum _format, GLenum _type, bool _isRenderTarget );
		~OGLTexture();

	public:
        unsigned char* lock( int* _pitch, const Rect& _rect, bool _readOnly = true ) override;
		void unlock() override;
	
	public:
		size_t getHWWidth() const override;
		size_t getHWHeight() const override;
        size_t getHWChannels() const override;

		PixelFormat getHWPixelFormat() const override;

	public:
		void destroy() override;

	public: 
		bool isRenderTarget() const;
		GLuint getUId() const;
		
		void setMinFilter( GLenum _minFilter );
        GLenum getMinFilter() const;

		void setMagFilter( GLenum _magFilter );
        GLenum getMagFilter() const;
		
		void setWrapS( GLenum _wrapS );
        GLenum getWrapS() const;

		void setWrapT( GLenum _wrapT );		
        GLenum getWrapT() const;

	private:
		GLuint m_uid;
		bool m_isRenderTarget;
		
		PixelFormat m_hwPixelFormat;

		size_t m_hwWidth;
		size_t m_hwHeight;
        size_t m_hwChannels;

		GLenum m_minFilter;
		GLenum m_magFilter;
		GLenum m_wrapS;
		GLenum m_wrapT;

		GLsizei m_pitch;

		GLint m_internalFormat;

		GLenum m_format;
		GLenum m_type;

		unsigned char* m_lock;
	};

}	// namespace Menge
