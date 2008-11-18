/*
 *  OGLRenderTexture.cpp
 *  Mac
 *
 *  Created by Berserk on 13.11.08.
 *  Copyright 2008 __Menge__. All rights reserved.
 *
 */

#include "OGLRenderTexture.h"
#include <cassert>

OGLRenderTexture::OGLRenderTexture()
: OGLTexture()
#ifdef MENGE_PLATFORM_MACOSX
, m_aglContext( 0 )
, m_aglPbuffer( 0 )
#endif
{
}

OGLRenderTexture::~OGLRenderTexture()
{
	if( m_framebufferID != 0 )
	{
		glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, m_framebufferID );
		glFramebufferTexture2DEXT( GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, 0, 0 );
		glFramebufferRenderbufferEXT( GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, 0 );
	
		glDeleteFramebuffersEXT( 1, &m_framebufferID );
	}	
	
	if( m_renderbufferID != 0 )
	{
		glDeleteRenderbuffersEXT( 1, &m_renderbufferID );
	}
}

bool OGLRenderTexture::create( const Menge::String& _name, std::size_t _width, std::size_t _height, std::size_t _data_width, std::size_t _data_height )
{
	if( aglCreatePBuffer( _width, _height, GL_TEXTURE_2D, GL_RGB, 0, m_aglPbuffer ) == GL_FALSE )
	{
		return false;
	}

	int i = 0;
	AGLPixelFormat pixelFormat;
	GLint attribs[ 20 ];

	attribs[ i++ ] = AGL_NO_RECOVERY;
	attribs[ i++ ] = GL_TRUE;
	attribs[ i++ ] = AGL_ACCELERATED;
	attribs[ i++ ] = GL_TRUE;
	attribs[ i++ ] = AGL_RGB;
	attribs[ i++ ] = AGL_DEPTH_SIZE;
	attribs[ i++ ] = 24;
	attribs[ i++ ] = AGL_NONE;
	pixelFormat = aglChoosePixelFormat( NULL, 0, attribs );
	m_aglContext = aglCreateContext(pixelFormat, NULL);

	if( m_aglContext == NULL )
	{
		return false;
	}

	glGenTextures( 1, m_texture );
	glBindTexture( GL_TEXTURE_2D, m_texture );

	if( aglTexImagePBuffer( m_aglContext, m_aglPbuffer, m_texture ) == GL_FALSE )
	{
		return false;
	}

	if( aglSetPBuffer( m_aglContext, m_aglPbuffer, 0, 0, 0 ) == GL_FALSE )
	{
		return false;
	}

	return true;
}

bool OGLRenderTexture::enable()
{

	aglSetCurrentContext( m_aglContext );
	
	return true;
}

unsigned char * OGLRenderTexture::lock( int* _pitch, bool _readOnly )
{
	assert(!"Not implemented");
	return NULL;
}

void OGLRenderTexture::unlock()
{
}
