/*
*	OGLTexture.h
*
*	Created by _Berserk_ on 9.10.2009
*	Copyright 2009 Menge. All rights reserved.
*
*/

#	pragma once

#	include "Interface/RenderSystemInterface.h"

#	include "OGLRenderSystem.h"

namespace Menge
{
	class OGLTexture 
		: public RenderImageInterface
	{
	public:
		OGLTexture( GLuint _uid, OGLRenderSystem* _renderSystem, size_t _width, size_t _height, size_t _requestedWidth, size_t _requestedHeight,
			int _pitch, PixelFormat _pixelFormat, GLint _internalFormat, GLenum _format, GLenum _type, bool _isRenderTarget );
		~OGLTexture();

	public:
        unsigned char* lock( int* _pitch, const Rect& _rect, bool _readOnly = true ) override;
		void unlock() override;
	
	public:
		size_t getHWWidth() const override;
		size_t getHWHeight() const override;

		PixelFormat getHWPixelFormat() const override;

	public:
		void destroy() override;

	public: 
		bool isRenderTarget() const;
		GLuint getUId() const;
		
		GLsizei getRequestedWidth() const;
		GLsizei getRequestedHeight() const;
		
		GLenum getMinFilter() const;
		void setMinFilter( GLenum _minFilter );

		GLenum getMagFilter() const;
		void setMagFilter( GLenum _magFilter );
		
		GLenum getWrapS() const;
		void setWrapS( GLenum _wrapS );

		GLenum getWrapT() const;
		void setWrapT( GLenum _wrapT );		

	private:
		GLuint m_uid;
		bool m_isRenderTarget;
		
		PixelFormat m_hwPixelFormat;

		size_t m_hwWidth;
		size_t m_hwHeight;

		GLsizei m_requestedWidth;
		GLsizei m_requestedHeight;

		GLenum m_minFilter;
		GLenum m_magFilter;
		GLenum m_wrapS;
		GLenum m_wrapT;

		GLsizei m_pitch;

		GLint m_internalFormat;

		GLenum m_format;
		GLenum m_type;

		unsigned char* m_lock;

		OGLRenderSystem* m_renderSystem;
	};

}	// namespace Menge
