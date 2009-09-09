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
, m_aglPbuffer( 0 )
, m_aglContext( 0 )
#endif
{
}

OGLRenderTexture::~OGLRenderTexture()
{
	if( m_texture != 0 )
	{
		glDeleteTextures( 1 ,&m_texture );
		m_texture = 0;
	}
	if( m_aglPbuffer != 0 )
	{
		aglDestroyPBuffer( m_aglPbuffer );
		m_aglPbuffer = 0;
	}
	if( m_aglContext != 0 )
	{
		aglDestroyContext( m_aglContext );
		m_aglContext = 0;
	}
}

bool OGLRenderTexture::create( const Menge::String& _name, std::size_t _width, std::size_t _height, std::size_t _data_width, std::size_t _data_height )
{
	AGLContext aglContext = aglGetCurrentContext();
	AGLPixelFormat pixelFormat;
	GLint attribs[] = { AGL_NO_RECOVERY, GL_TRUE, AGL_ACCELERATED, GL_TRUE, AGL_RGBA, AGL_ALPHA_SIZE, 8
						,AGL_RED_SIZE, 8, AGL_STENCIL_SIZE, 8, AGL_DEPTH_SIZE, 24, AGL_NONE };
						
	pixelFormat = aglChoosePixelFormat( NULL, 0, attribs );
	m_aglContext = aglCreateContext( pixelFormat, aglContext );
	
	if( m_aglContext == NULL )
	{
		return false;
	}
	
	if( aglCreatePBuffer( _data_width, _data_height, GL_TEXTURE_2D, GL_RGB, 0, &m_aglPbuffer ) == GL_FALSE )
	{
		return false;
	}

	aglSetCurrentContext( NULL );
	aglSetCurrentContext( m_aglContext );
	
	glGenTextures( 1, &m_texture );
	glBindTexture( GL_TEXTURE_2D, m_texture );
	
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0 );
	glTexParameteri( GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_FALSE );
	
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
	
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );

	if( aglSetPBuffer( m_aglContext, m_aglPbuffer, 0, 0, 0 ) == false )
	{
		return false;
	}
	

	if( aglTexImagePBuffer( m_aglContext, m_aglPbuffer, GL_FRONT ) == GL_FALSE )
	{
		//printf( "%d\n", aglGetError() );
		return false;
	}
	glBindTexture( GL_TEXTURE_2D, 0 );
	
	aglSetCurrentContext( NULL );
	aglSetCurrentContext( aglContext );
	

	
	m_format = Menge::PF_R8G8B8;
	m_name = _name;
	m_width = _data_width;
	m_height = _data_height;
	m_image_width = _width;
	m_image_height = _height;
	m_uvMask[0] = 1.0f;//static_cast<float>( m_image_width ) / m_width;
	m_uvMask[1] = 1.0f;//static_cast<float>( m_image_height ) / m_height;
	m_pitch = 0;//_desc.size / _desc.height;
	return true;
}

bool OGLRenderTexture::enable()
{

	AGLContext aglContext = aglGetCurrentContext();

	if( aglSetCurrentContext( NULL ) == GL_FALSE )
	{
		printf( "ERROR\n" );
	}
	if( aglSetCurrentContext( m_aglContext ) == GL_FALSE )
	{
		printf( "ERROR2\n" );
	}

	//aglSetCurrentContext( aglContext );

	// update and draw pbuffer content for current target context (this ensures we are actually drawing on the same renderer)
	/*GLint vs = aglGetVirtualScreen(aglContext);
	//aglSetDrawable( aglContext, NULL );
	if( !aglSetPBuffer( m_aglContext, m_aglPbuffer, 0, 0, vs) )
	{
		return false;
	}*/
	
	/*glViewport( 0, 0, m_width, m_height );
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	glOrtho( 0, m_width, 0, m_height, -1, 1 );*/
	aglSetPBuffer( m_aglContext, m_aglPbuffer, 0, 0, 0);

	glClearColor( 1.0f, 0.0f, 0.0f, 1.0f );
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//aglSwapBuffers( m_aglContext );
	//glBindTexture( GL_TEXTURE_2D, m_texture );
	//aglTexImagePBuffer( m_aglContext, m_aglPbuffer, GL_FRONT );
	//glBindTexture( GL_TEXTURE_2D, 0 );
	//aglSetCurrentContext( NULL );
	//aglSetCurrentContext( aglContext );
	
	return true;
}

void OGLRenderTexture::flush()
{
	//AGLContext aglContext = aglGetCurrentContext();
	//aglSwapBuffers( aglContext );
	aglSwapBuffers( m_aglContext );
	aglSetCurrentContext( NULL );
}

unsigned char * OGLRenderTexture::lock( int* _pitch, bool _readOnly )
{
	assert(!"Not implemented");
	return NULL;
}

void OGLRenderTexture::unlock()
{
}
