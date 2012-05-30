/*
 *	DX8Texture.h
 *
 *	Created by _Berserk_ on 22.1.2009
 *	Copyright 2009 Menge. All rights reserved.
 *
 */

#	pragma once

#	include "Interface/RenderSystemInterface.h"

struct IDirect3DTexture8;

namespace Menge
{
	class DX8Texture
		: public RenderImageInterface
	{
	public:
		DX8Texture( IDirect3DTexture8 * _d3dInterface, size_t _width, size_t _height );
		virtual ~DX8Texture();

	public:
		IDirect3DTexture8 * getInterface() const;		

	public:
		size_t getHWWidth() const;
		size_t getHWHeight() const;

	public:
        unsigned char * lock( int* _pitch, const Rect& _rect, bool _readOnly ) override;
		void unlock() override;

	public:
		void destroy() override;
        		
	public:
		virtual void release();

	protected:
		IDirect3DTexture8* m_d3dInterface;

		size_t m_width;
		size_t m_height;

		size_t m_hwWidth;
		size_t m_hwHeight;
	};
}	// namespace Menge
