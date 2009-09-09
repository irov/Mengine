/*
 *  OGLRenderTextureFBO.cpp
 *  Mac
 *
 *  Created by Berserk on 13.11.08.
 *  Copyright 2008 __Menge__. All rights reserved.
 *
 */

#include "OGLRenderTextureFBO.h"
#include <cassert>

OGLRenderTextureFBO::OGLRenderTextureFBO()
: OGLTexture()
, m_framebufferID( 0 )
, m_renderbufferID( 0 )
{
}

OGLRenderTextureFBO::~OGLRenderTextureFBO()
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

bool OGLRenderTextureFBO::create( const Menge::String& _name, std::size_t _width, std::size_t _height, std::size_t _data_width, std::size_t _data_height )
{
	// Создаём буфер кадра.
	glGenFramebuffersEXT( 1, &m_framebufferID );
	glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, m_framebufferID );

	// Создаём текстуру.
	glGenTextures( 1, &m_texture );
	glBindTexture( GL_TEXTURE_2D, m_texture );
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB8, _data_width, _data_height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR ); 
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );

	// Создаём буфер вывода.
	glGenRenderbuffersEXT( 1, &m_renderbufferID );
	glBindRenderbufferEXT( GL_RENDERBUFFER_EXT, m_renderbufferID );
	glRenderbufferStorageEXT( GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT24, _data_width, _data_height );

	// Присоединяем текстуру как буфер цвета, а буфер вывода - как буфер глубины.
	glFramebufferTexture2DEXT( GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, m_texture, 0 );
	glFramebufferRenderbufferEXT( GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, m_renderbufferID );

	// Обрабатываем возможные ошибки.
	GLenum status = glCheckFramebufferStatusEXT( GL_FRAMEBUFFER_EXT );
	if( status != GL_FRAMEBUFFER_COMPLETE_EXT || glGetError() != GL_NO_ERROR )
	{
		glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, 0, 0);
		glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, 0);

		glBindTexture( GL_TEXTURE_2D, 0 );
		glDeleteTextures( 1, &m_texture );
		m_texture = 0;
		glBindRenderbufferEXT( GL_RENDERBUFFER_EXT, 0 );
		glDeleteRenderbuffersEXT( 1, &m_renderbufferID );
		m_renderbufferID = 0;
		glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, 0 );
		glDeleteFramebuffersEXT( 1, &m_framebufferID );
		m_framebufferID = 0;
		return false;
	}
	
	glBindRenderbufferEXT( GL_RENDERBUFFER_EXT, 0 );
	glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, 0 );
	glBindTexture( GL_TEXTURE_2D, 0 );
	
	m_name = _name;
	m_image_width = _width;
	m_image_height = _height;
	m_width = _data_width;
	m_height = _data_height;
	m_uvMask[0] = static_cast<float>( m_image_width ) / m_width;
	m_uvMask[1] = static_cast<float>( m_image_height ) / m_height;

	return true;
}

bool OGLRenderTextureFBO::enable()
{
	if( m_framebufferID == 0 )
	{
		return false;
	}
	
	glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, m_framebufferID );
	glDrawBuffer( GL_COLOR_ATTACHMENT0_EXT );
	glReadBuffer( GL_COLOR_ATTACHMENT0_EXT );
	
	return true;
}

void OGLRenderTextureFBO::flush()
{
	//glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, m_framebufferID );
	//AGLContext context = aglGetCurrentContext();
	//aglSwapBuffers( context );
}

unsigned char * OGLRenderTextureFBO::lock( int* _pitch, bool _readOnly )
{
	assert(!"Not implemented");
	return NULL;
}

void OGLRenderTextureFBO::unlock()
{
}
