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

#	include "Core/ConstString.h"

namespace Menge
{
	class ImageDecoderInterface;

	class Texture
		: public TextureInterface
	{
	public:
		Texture( RenderImageInterface* _interface
				, const ConstString & _name
				, size_t _width
				, size_t _height
				, PixelFormat _format
				, size_t _hwWidth
				, size_t _hwHeight
				, PixelFormat _hwPixelFormat
				, int _id );

		~Texture();
		
		RenderImageInterface* getInterface() const override;
		const ConstString & getName() const override;
		size_t getWidth() const override;
		size_t getHeight() const override;
		PixelFormat getPixelFormat() const override;

		size_t addRef() const override;
		size_t decRef() const override;
		
		unsigned char* lock( int* _pitch, bool _readOnly = true ) const override;
		unsigned char* lockRect( int* _pitch, const Rect& _rect, bool _readOnly = true ) const override;
		void unlock() const override;

		size_t getHWWidth() const override;
		size_t getHWHeight() const override;
		PixelFormat getHWPixelFormat() const override;
		const mt::mat4f* getUVMask() const override;

		bool loadImageData( ImageDecoderInterface* _imageDecoder ) override;
		bool loadImageData( unsigned char* _textureBuffer, size_t _texturePitch, ImageDecoderInterface* _imageDecoder ) override;

		int getID() const;

	protected:
		mutable size_t m_ref;

		RenderImageInterface* m_iTexture;
		ConstString m_name;
		size_t m_width;
		size_t m_height;
		PixelFormat m_pixelFormat;
		size_t m_hwWidth;		// hardware width
		size_t m_hwHeight;		// hardware height
		PixelFormat m_hwPixelFormat;	// hardware pixel format
		mt::mat4f* m_uvMask;
		int m_id;
	};
}	// namespace Menge
