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
		OGLTexture( GLuint _uid, OGLRenderSystem* _renderSystem );
		~OGLTexture();

	public:
		unsigned char* lock( int* _pitch, bool _readOnly /* = true  */ ) override;
		void unlock() override;

	public:
		GLuint uid;
		GLint level;
		GLint internalFormat;
		GLsizei width;
		GLsizei height;
		GLint border;
		GLenum format;
		GLenum type;
		GLenum minFilter;
		GLenum magFilter;
		GLenum wrapS;
		GLenum wrapT;
		unsigned char* m_lock;
		GLsizei pitch;
		GLsizei requestedWidth;
		GLsizei requestedHeight;
		bool m_isRenderTarget;

	private:
		OGLRenderSystem* m_renderSystem;
	};

}	// namespace Menge
