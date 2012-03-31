#	pragma once
#	include "Resource.h"
#	include "ResourceImage.h"

#	include "Utils/Core/PixelFormat.h"
#	include "Utils/Core/Rect.h"

namespace Menge
{
	class ImageDecoderInterface;

	class ResourceImageAtlas
		: public ResourceReference
	{
		RESOURCE_DECLARE( ResourceImageAtlas )

	public:
		ResourceImageAtlas();

	public:
		bool loadFrame( ImageDecoderInterface * _decoder, ResourceImage::ImageFrame & _frame );
		RenderTextureInterface* getTexture() const;
		void loader( BinParser * _parser ) override;

	protected:
		void _getRectForUV( Rect& _destRect, const mt::vec4f & _uv );
		bool _compile() override;
		void _release() override;

		Menge::PixelFormat m_pixelFormat;
		mt::vec2f m_size;
		RenderTextureInterface * m_texture;
	};
}
