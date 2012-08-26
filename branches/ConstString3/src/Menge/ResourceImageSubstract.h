#	pragma once

#	include "ResourceImageDefault.h"
#	include "Config/Typedef.h"

#	include <vector>

namespace Menge
{	
	class ResourceImageSubstract
		: public ResourceImageDefault
	{
		RESOURCE_DECLARE( ResourceImageSubstract )

	public:
		ResourceImageSubstract();

	public:
		RenderTextureInterface * getTexture() const override;

	protected:
		void loader( const Metabuf::Metadata * _parser ) override;

	protected:
		bool _compile() override;
		void _release() override;

    protected:
		ResourceImage * m_resourceImage;
		ConstString m_resourceImageName;

        mt::vec4f m_uv;
	};
}
