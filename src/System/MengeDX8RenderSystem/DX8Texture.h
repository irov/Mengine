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
		DX8Texture( IDirect3DTexture8 * _d3dInterface, size_t _hwWidth, size_t _hwHeight, size_t _hwChannels, PixelFormat _hwPixelFormat );
		virtual ~DX8Texture();

	public:
		IDirect3DTexture8 * getInterface() const;		

	public:
		size_t getHWWidth() const override;
		size_t getHWHeight() const override;
        size_t getHWChannels() const override;

        PixelFormat getHWPixelFormat() const override;
        

	public:
        unsigned char * lock( int* _pitch, const Rect& _rect, bool _readOnly ) override;
		void unlock() override;

	public:
		void destroy() override;
        		
	public:
		virtual void release();

	protected:
		IDirect3DTexture8* m_d3dInterface;

		size_t m_hwWidth;
		size_t m_hwHeight;
        size_t m_hwChannels;

        PixelFormat m_hwPixelFormat;        
	};
}	// namespace Menge
