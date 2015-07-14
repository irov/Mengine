#	pragma once

#	include "Kernel/ResourceReference.h"

#	include "spine/spine.h"

namespace Menge
{
	class ResourceSpine
		: public ResourceReference
	{
		RESOURCE_DECLARE( ResourceSpine )

	public:
		ResourceSpine();
		~ResourceSpine();

	public:
		const RenderTextureInterfacePtr & getAtlasTexture() const;

	public:
		spAtlas * getAtlas();
		spSkeletonData * getSkeletonData();

	public:
		bool _loader( const Metabuf::Metadata * _meta ) override;

	protected:
		bool _compile() override;
		void _release() override;

	public:
		bool _isValid() const override;

	public:
		FilePath m_skeletonPath;
		FilePath m_atlasPath;
		FilePath m_texturePath;
		ConstString m_textureCodecName;

		RenderTextureInterfacePtr m_texture;

		spAtlas * m_atlas;
		spSkeletonData * m_skeletonData;
	};
}
