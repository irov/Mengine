#	pragma once

#	include "ResourceImageDefault.h"

#	include "Config/Typedef.h"

#	include <stdex/stl_vector.h>

namespace Menge
{	
	class ResourceImageSubstract
		: public ResourceImage
	{
		RESOURCE_DECLARE( ResourceImageSubstract )

	public:
		ResourceImageSubstract();

	protected:
		bool _loader( const Metabuf::Metadata * _parser ) override;

	protected:
		bool _compile() override;
		void _release() override;

    protected:
		ResourceImage * m_resourceImage;
		ConstString m_resourceImageName;
	};
}
