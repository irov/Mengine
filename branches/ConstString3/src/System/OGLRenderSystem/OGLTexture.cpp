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
	OGLTexture::OGLTexture( GLuint _uid, OGLRenderSystem* _renderSystem )
		: uid(_uid)
		, m_renderSystem(_renderSystem)
		, m_lock(NULL)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	OGLTexture::~OGLTexture()
	{
	}
    //////////////////////////////////////////////////////////////////////////
    unsigned char* OGLTexture::lockRect( int* _pitch, const Rect& _rect, bool _readOnly )
    {
		m_lock = new unsigned char[width*height*numColors];
		*_pitch = static_cast<int>(pitch);

		return m_lock;
    }
	//////////////////////////////////////////////////////////////////////////
	void OGLTexture::unlock()
	{
		glBindTexture( GL_TEXTURE_2D, uid );

		glTexImage2D( GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, type, m_lock );

		glBindTexture( GL_TEXTURE_2D, m_activeTexture );

		delete [] m_lock;
		m_lock = NULL;
	}
	//////////////////////////////////////////////////////////////////////////
	void OGLTexture::destroy() override
	{
		glDeleteTextures( 1, &uid );

		m_renderSystem->checkActiveTexture( uid );

		delete this;
	}
}	// namespace Menge
