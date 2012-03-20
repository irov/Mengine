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
		: uid( _uid )
		, m_renderSystem( _renderSystem )
		, m_lock( NULL )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	OGLTexture::~OGLTexture()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	unsigned char* OGLTexture::lock( int* _pitch, bool _readOnly /* = true */ )
	{
		*_pitch = static_cast<int>( pitch );
		return m_lock;
	}
    //////////////////////////////////////////////////////////////////////////
    unsigned char* OGLTexture::lockRect( int* _pitch, const Rect& _rect, bool _readOnly )
    {
        return NULL;
    }
	//////////////////////////////////////////////////////////////////////////
	void OGLTexture::unlock()
	{
		m_renderSystem->unlockTexture( uid, internalFormat, width, height, format, type, m_lock );
	}
	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge
