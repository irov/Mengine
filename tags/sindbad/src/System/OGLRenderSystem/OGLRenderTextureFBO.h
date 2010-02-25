/*
 *  OGLRenderTextureFBO.h
 *  Mac
 *
 *  Created by Berserk on 13.11.08.
 *  Copyright 2008 __Menge__. All rights reserved.
 *
 */

#	pragma once

#	include "OGLTexture.h"

class OGLRenderTextureFBO
	: public OGLTexture
{
public:
	OGLRenderTextureFBO();
	~OGLRenderTextureFBO();
	
	bool create( const Menge::String& _name, std::size_t _width, std::size_t _height, std::size_t _data_width, std::size_t _data_height );
	bool enable();
	
	void flush();
	
	virtual unsigned char * lock( int* _pitch, bool _readOnly = true )  override;
	virtual void unlock() override;
	
protected:
	GLuint m_framebufferID;
	GLuint m_renderbufferID;
	
};

