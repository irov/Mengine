#	pragma once

#	include "ResourceImageDefault.h"
#	include "Config/Typedef.h"

#	include <vector>

namespace Menge
{
	class ResourceImageAtlas;

	class ResourceImageInAtlasDefault
		: public ResourceImageDefault
	{
		RESOURCE_DECLARE( ResourceImageInAtlasDefault )

	public:
		ResourceImageInAtlasDefault();

	public:
		RenderTextureInterface * getTexture() const override;

	protected:
		void loader( const Metabuf::Metadata * _parser ) override;

	protected:
		bool _compile() override;
		void _release() override;

		ResourceImageAtlas * m_resourceAtlas;
		ConstString m_resourceAtlasName;
	};
}
