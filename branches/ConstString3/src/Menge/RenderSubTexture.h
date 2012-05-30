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
	class ImageDecoderInterface;

	class RenderSubTexture
		: public RenderTextureInterface
	{
	public:
		RenderSubTexture( RenderTextureInterface * _texture
				, const Rect & _rect
				, size_t _id
				, RenderTextureInterfaceListener * _listener				
				);

		~RenderSubTexture();
		
	public:
		RenderImageInterface* getImage() const override;
		void destroyImage() override;

		size_t getId() const override;

		size_t addRef() const override;
		size_t decRef() const override;

		const Rect & getRect() const override;
		const Rect & getHWRect() const override;

		const mt::vec4f & getUV() const override;

		void setFileName( const WString & _filename ) override;
		const WString & getFileName() const override;

		size_t getWidth() const override;
		size_t getHeight() const override;

		PixelFormat getPixelFormat() const override;
		
		size_t getMemoryUse() const override;

		unsigned char* lock( int* _pitch, const Rect& _rect, bool _readOnly = true ) const override;
		void unlock() const override;

		size_t getHWWidth() const override;
		size_t getHWHeight() const override;
		PixelFormat getHWPixelFormat() const override;

	protected:
		RenderTextureInterface * m_texture;
		Rect m_rect;
		Rect m_hwRect;

		RenderTextureInterfaceListener * m_listener;

		mutable size_t m_ref;
		mt::vec4f m_uv;

		WString m_fileName;

		size_t m_id;
	};
}	// namespace Menge
