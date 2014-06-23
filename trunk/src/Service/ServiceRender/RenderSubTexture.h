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
		RenderSubTexture();
		~RenderSubTexture();

    public:
        void initialize( const RenderTextureInterfacePtr & _texture
            , const Rect & _rect
            , size_t _id
            );		

	public:
		void release() override;
		
	public:
		const RenderImageInterfacePtr & getImage() const override;

		size_t getId() const override;

		const Rect & getRect() const override;
		const Rect & getHWRect() const override;

		const mt::vec4f & getUV() const override;

		void setFileName( const FilePath & _fileName ) override;
		const FilePath & getFileName() const override;

		size_t getWidth() const override;
		size_t getHeight() const override;
        size_t getChannels() const override;
		
		size_t getMemoryUse() const override;

		unsigned char* lock( int* _pitch, const Rect& _rect, bool _readOnly = true ) const override;
		void unlock() const override;

	protected:
		RenderTextureInterfacePtr m_texture;
		Rect m_rect;
		Rect m_hwRect;

        mt::vec4f m_uv;

		FilePath m_fileName;

		size_t m_id;
	};
}	// namespace Menge
