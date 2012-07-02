#	pragma once

#	include "ResourceImageCombineRGBAndAlpha.h"
#	include "Config/Typedef.h"

#	include <vector>

namespace Menge
{
	class ResourceImageAtlas;

	class ResourceImageInAtlasCombineRGBAndAlpha
		: public ResourceImageCombineRGBAndAlpha
	{
		RESOURCE_DECLARE( ResourceImageInAtlasCombineRGBAndAlpha )

	public:
		ResourceImageInAtlasCombineRGBAndAlpha();

	public:
		RenderTextureInterface * getTexture() const override;
	protected:
		void loader( BinParser * _parser ) override;

	protected:
		bool _compile() override;
		void _release() override;

		ResourceImageAtlas * m_resourceAtlas;
		ConstString m_resourceAtlasName;
	};
}
