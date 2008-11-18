/*
 *  OGLRenderTexture.h
 *  Mac
 *
 *  Created by Berserk on 13.11.08.
 *  Copyright 2008 __Menge__. All rights reserved.
 *
 */

#	pragma once

#	include "OGLTexture.h"

class OGLRenderTexture
	: public OGLTexture
{
public:
	OGLRenderTexture();
	~OGLRenderTexture();
	
	bool create( const Menge::String& _name, std::size_t _width, std::size_t _height, std::size_t _data_width, std::size_t _data_height );
	bool enable();
	
	virtual unsigned char * lock( int* _pitch, bool _readOnly = true )  override;
	virtual void unlock() override;
	
protected:
#ifdef MENGE_PLATFORM_MACOSX
	AGLPbuffer* m_aglPbuffer;
	AGLContext* m_aglContext;
#endif
};

