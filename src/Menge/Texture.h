/*
 *	Texture.h
 *
 *	Created by _Berserk_ on 12.3.2009
 *	Copyright 2009 Menge. All rights reserved.
 *
 */

#	pragma once

#	include "Interface/RenderSystemInterface.h"
#	include "math/mat4.h"

namespace Menge
{
	class ImageDecoderInterface;

	class Texture
	{
	public:
		Texture( RenderImageInterface* _interface
				, const String& _name
				, size_t _width
				, size_t _height
				, PixelFormat _format
				, size_t _hwWidth
				, size_t _hwHeight
				, PixelFormat _hwPixelFormat );
		~Texture();

		RenderImageInterface* getInterface();
		const String& getName() const;
		size_t getWidth() const;
		size_t getHeight() const;
		PixelFormat getPixelFormat() const;

		size_t addRef();
		size_t decRef();

		unsigned char* lock( int* _pitch, bool _readOnly = true );
		void unlock();

		size_t getHWWidth() const;
		size_t getHWHeight() const;
		PixelFormat getHWPixelFormat() const;
		const mt::mat4f* getUVMask() const;

		void loadImageData( ImageDecoderInterface* _imageDecoder );
		void loadImageData( unsigned char* _textureBuffer, int _texturePitch, ImageDecoderInterface* _imageDecoder );

	protected:
		size_t m_ref;
		RenderImageInterface* m_iTexture;
		String m_name;
		size_t m_width;
		size_t m_height;
		PixelFormat m_pixelFormat;
		size_t m_hwWidth;		// hardware width
		size_t m_hwHeight;		// hardware height
		PixelFormat m_hwPixelFormat;	// hardware pixel format
		mt::mat4f* m_uvMask;
	};
}	// namespace Menge
