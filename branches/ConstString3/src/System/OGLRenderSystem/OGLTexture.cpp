/*
 *	OGLTexture.cpp
 *
 *	Created by _Berserk_ on 9.10.2009
 *	Copyright 2009 Menge. All rights reserved.
 *
 */

#	include "OGLTexture.h"


namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	OGLTexture::OGLTexture( GLuint _uid, OGLRenderSystem* _renderSystem, size_t _width, size_t _height, size_t _requestedWidth, size_t _requestedHeight,
		int _pitch, PixelFormat _pixelFormat, GLint _internalFormat, GLenum _format, GLenum _type, bool _isRenderTarget )
		: m_uid(_uid)
		, m_renderSystem(_renderSystem)
		, m_hwWidth(_width)
		, m_hwHeight(_height)
		, m_requestedWidth(_requestedWidth)
		, m_requestedHeight(_requestedHeight)
		, m_pitch(_pitch)
		, m_hwPixelFormat(_pixelFormat)
		, m_internalFormat(_internalFormat)
		, m_format(_format)
		, m_type(_type)
		, m_isRenderTarget(_isRenderTarget)
		, m_lock(NULL)
		, m_wrapS(0)
		, m_wrapT(0)
		, m_minFilter(0)
		, m_magFilter(0)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	OGLTexture::~OGLTexture()
	{
	}
    //////////////////////////////////////////////////////////////////////////
    unsigned char* OGLTexture::lock( int* _pitch, const Rect& _rect, bool _readOnly )
    {
		m_lock = new unsigned char[m_hwHeight * m_pitch];
		*_pitch = static_cast<int>(m_pitch);

		return m_lock;
    }
	//////////////////////////////////////////////////////////////////////////
	size_t OGLTexture::getHWWidth() const 
	{
		return m_hwWidth;
	}
	//////////////////////////////////////////////////////////////////////////
	size_t OGLTexture::getHWHeight() const 
	{
		return m_hwHeight;
	}
	//////////////////////////////////////////////////////////////////////////
	PixelFormat OGLTexture::getHWPixelFormat() const 
	{
		return m_hwPixelFormat;
	}
	//////////////////////////////////////////////////////////////////////////
	void OGLTexture::unlock()
	{
		glBindTexture( GL_TEXTURE_2D, m_uid );

		glTexImage2D( GL_TEXTURE_2D, 0, m_internalFormat, m_hwWidth, m_hwHeight, 0, m_format, m_type, m_lock );

		glBindTexture( GL_TEXTURE_2D, m_uid );

		delete [] m_lock;
		m_lock = NULL;
	}
	//////////////////////////////////////////////////////////////////////////
	void OGLTexture::destroy()
	{
		glDeleteTextures( 1, &m_uid );

		m_renderSystem->checkActiveTexture( m_uid );

		delete this;
	}
	//////////////////////////////////////////////////////////////////////////
	bool OGLTexture::isRenderTarget() const
	{
		return m_isRenderTarget;
	}
	//////////////////////////////////////////////////////////////////////////
	GLuint OGLTexture::getUId() const
	{
		return m_uid;
	}

	GLenum OGLTexture::getMinFilter() const
	{
		return m_minFilter;
	}
	//////////////////////////////////////////////////////////////////////////
	void OGLTexture::setMinFilter( GLenum _minFilter )
	{
		m_minFilter = _minFilter;
	}
	//////////////////////////////////////////////////////////////////////////
	GLenum OGLTexture::getMagFilter() const
	{
		return m_magFilter;
	}
	//////////////////////////////////////////////////////////////////////////
	void OGLTexture::setMagFilter( GLenum _magFilter )
	{
		m_magFilter = _magFilter;
	}
	//////////////////////////////////////////////////////////////////////////
	GLenum OGLTexture::getWrapS() const
	{
		return m_wrapS;
	}
	//////////////////////////////////////////////////////////////////////////
	void OGLTexture::setWrapS( GLenum _wrapS )
	{
		m_wrapS = _wrapS;
	}
	//////////////////////////////////////////////////////////////////////////
	GLenum OGLTexture::getWrapT() const
	{
		return m_wrapT;
	}
	//////////////////////////////////////////////////////////////////////////
	void OGLTexture::setWrapT( GLenum _wrapT )
	{
		m_wrapT = _wrapT;
	}
	//////////////////////////////////////////////////////////////////////////
	GLsizei OGLTexture::getRequestedWidth() const
	{
		return m_requestedWidth;
	}
	//////////////////////////////////////////////////////////////////////////
	GLsizei OGLTexture::getRequestedHeight() const
	{
		return m_requestedHeight;
	}
	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge
