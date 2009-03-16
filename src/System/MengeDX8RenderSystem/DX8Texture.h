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
		DX8Texture( IDirect3DTexture8* _d3dInterface, 
			std::size_t _width, std::size_t _height, PixelFormat _format );
		virtual ~DX8Texture();

		IDirect3DTexture8* getInterface();

		void loadData( ImageDecoderInterface* _imageDecoder );

		size_t getWidth() const;
		size_t getHeight() const;

	public:
		unsigned char* lock( int* _pitch, bool _readOnly = true ) override;
		void unlock() override;

	protected:
		IDirect3DTexture8* m_d3dInterface;
		std::size_t m_width;
		std::size_t m_height;
		PixelFormat m_format;
		size_t m_hwWidth;
		size_t m_hwHeight;
	};
}	// namespace Menge
