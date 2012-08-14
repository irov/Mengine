/*
 *	Texture.h
 *
 *	Created by _Berserk_ on 12.3.2009
 *	Copyright 2009 Menge. All rights reserved.
 *
 */

#	pragma once

#	include "Interface/RenderSystemInterface.h"
#	include "math/vec4.h"

#	include "Core/ConstString.h"

namespace Menge
{
	class RenderTexture
		: public RenderTextureInterface
	{
	public:
		RenderTexture( RenderImageInterface* _image
				, size_t _width
				, size_t _height
				, PixelFormat _format
				, size_t _hwWidth
				, size_t _hwHeight
				, PixelFormat _hwPixelFormat
				, size_t _id 
				);

		~RenderTexture();
		
	public:
		RenderImageInterface* getImage() const override;

		void destroy() override;

		size_t getId() const override;

		size_t addRef() const override;
		size_t decRef() const override;

		void setFileName( const WString & _filename ) override;
		const WString & getFileName() const override;
		
		const Rect & getRect() const override;
		const Rect & getHWRect() const override;

		const mt::vec4f & getUV() const override;

		size_t getWidth() const override;
		size_t getHeight() const override;

		PixelFormat getPixelFormat() const override;
		
		unsigned char* lock( int* _pitch, const Rect& _rect, bool _readOnly = true ) const override;

		void unlock() const override;

		size_t getHWWidth() const override;
		size_t getHWHeight() const override;
		PixelFormat getHWPixelFormat() const override;

		size_t getMemoryUse() const override;

	protected:
		mutable size_t m_ref;

		RenderImageInterface* m_image;
		
		WString m_filename;
		
		size_t m_width;
		size_t m_height;
		PixelFormat m_pixelFormat;
		
		size_t m_hwWidth;		// hardware width
		size_t m_hwHeight;		// hardware height
		PixelFormat m_hwPixelFormat;	// hardware pixel format

		Rect m_rect;
		mt::vec4f m_uv;

		size_t m_id;
	};
}	// namespace Menge
