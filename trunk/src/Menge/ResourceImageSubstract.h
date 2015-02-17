#	pragma once

#	include "ResourceImageDefault.h"

#	include "Config/Typedef.h"

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
		ConstString m_resourceImageName;
		ResourceImage * m_resourceImage;		
	};
}
