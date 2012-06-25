#	pragma once
#	include "Resource.h"
#	include "ResourceImage.h"

#	include "Utils/Core/PixelFormat.h"
#	include "Utils/Core/Rect.h"

namespace Menge
{
	class ImageDecoderInterface;
	class ResourceImageInAtlas;

	class ResourceImageAtlas
		: public ResourceReference
	{
		RESOURCE_DECLARE( ResourceImageAtlas )

	public:
		ResourceImageAtlas();

	public:
		bool setImageInAtlas( ResourceImageInAtlas * _resource, ResourceImage::ImageFrame & _frame );
		RenderTextureInterface* getTexture() const;
		void loader( BinParser * _parser ) override;

	protected:		
		bool _compile() override;
		void _release() override;

	protected:
		void getRectForUV_( Rect& _destRect, const mt::vec4f & _uv );

	protected:
		PixelFormat m_pixelFormat;
		mt::vec2f m_size;
		RenderTextureInterface * m_texture;
	};
}
